// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeActor.h"
#include "Maze/CellActor.h"
#include "Maze/MazeData.h"
#include "Maze/CellData.h"

#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "DrawDebugHelpers.h"

AMazeActor::AMazeActor()
{
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    FloorHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("FloorHISM"));
    FloorHISM->SetupAttachment(Root);
    FloorHISM->SetCollisionProfileName(TEXT("BlockAll"));

    WallHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("WallHISM"));
    WallHISM->SetupAttachment(Root);
    WallHISM->SetCollisionProfileName(TEXT("BlockAll"));

    GateHISM = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("GateHISM"));
    GateHISM->SetupAttachment(Root);
    GateHISM->SetCollisionProfileName(TEXT("BlockAllDynamic"));
}

void AMazeActor::GenerateMaze()
{
    OnPreMazeGenerate();
    ClearMaze();

    TUniquePtr<FMazeData> Maze = MakeMazeData();
    if (bUseHISM)
    {
        BuildMazeHISM(Maze.Get());
    }
    else
    {
        SpawnCells(Maze.Get());
    }

    OnPostMazeGenerate();

    /*
    UE_LOG(LogTemp, Log, TEXT("Generate %dx%d Maze."), Width, Height);

    if (!GetWorld())
    {
        UE_LOG(LogTemp, Error, TEXT("GetWorld() Error!"), Width, Height);
        return;
    }

    constexpr float CellSize = 1000.0f;

    if (!CellActors.IsEmpty())
    {
        for (auto& Cell : CellActors)
        {
            if (IsValid(Cell))
            {
                Cell->Destroy();
            }
        }

        CellActors.Empty();
    }
    CellActors.Reserve(Width * Height);

    FMazeData MazeData;
    MazeData.MakeMaze(Width, Height, RandomSeed);

    FTransform Transform = GetActorTransform();
    for (int i = 0; i < Width; i++)
    {
        for (int j = 0; j < Height; j++)
        {
            Transform.SetLocation(FVector(CellSize * (Height - (2 * j + 1)), CellSize * -(Width - (2 * i + 1)), 0.0f));
            ACellActor* Spawned = GetWorld()->SpawnActor<ACellActor>(CellActorClass, Transform);

            if (Spawned)
            {
                Spawned->InitializeCell(MazeData.GetCell(i, j));
                CellActors.Add(Spawned);
            }
        }
    }
    */
}

void AMazeActor::ClearMaze()
{
    if (bUseHISM)
    {
        ClearHISMInstances();
    }
    else
    {
        ClearSpawnedCells();
    }
}

void AMazeActor::BeginPlay()
{
    Super::BeginPlay();
    GenerateMaze();
}

void AMazeActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMazeActor::OnPreMazeGenerate()
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSystem)
    {
        NavSystem->SetNavigationAutoUpdateEnabled(false, NavSystem); // NavMesh 자동 업데이트 방지
    }
}

void AMazeActor::OnPostMazeGenerate()
{
    UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSystem)
    {
        NavSystem->Build(); // NavMesh 수동 생성
        NavSystem->SetNavigationAutoUpdateEnabled(true, NavSystem); // NavMesh 자동 업데이트 재개
    }
}

TUniquePtr<FMazeData> AMazeActor::MakeMazeData()
{
    TUniquePtr<FMazeData> Maze = MakeUnique<FMazeData>();
    Maze->MakeMaze(static_cast<uint8>(Width), static_cast<uint8>(Width), RandomSeed, MazeAlgorithmType);
    return Maze;
}

void AMazeActor::SpawnCells(FMazeData* InMaze)
{
    if (!CellActorClass)
    {
        return;
    }

    CellActors.Reserve(Width * Height);

    const ACellActor* DefaultCell = CellActorClass->GetDefaultObject<ACellActor>();

    CellHalfSize = DefaultCell->GetCellHalfSize();
    float CellSize = CellHalfSize * 2.0f;

    FVector StartLocation = FVector(Height * CellHalfSize, -Width * CellHalfSize, 0.0f)
        + FVector((Height % 2) * -CellHalfSize, (Width % 2) * CellHalfSize, 0.0f);

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // x, y는 MazeData의 좌표계 기준
    for (uint8 y = 0; y < Height; y++)
    {
        for (uint8 x = 0; x < Width; x++)
        {
            FCellData* Cell = InMaze->GetCell(x, y);
            if (!Cell)
            {
                continue;
            }

            FVector CellLocation = StartLocation + FVector(-y * CellSize, x * CellSize, 0.0f);
            ACellActor* CellActor = World->SpawnActor<ACellActor>(
                CellActorClass,
                CellLocation,
                FRotator::ZeroRotator
            );

            if (CellActor)
            {
                CellActor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
                CellActor->InitializeCell(Cell);
                CellActors.Add(CellActor);
            }
        }
    }
}

void AMazeActor::ClearSpawnedCells()
{
    //UE_LOG(LogTemp, Log, TEXT("Remove Maze."));

    for (ACellActor* Cell : CellActors)
    {
        if (IsValid(Cell))
        {
            Cell->Destroy();
        }
    }

    CellActors.Empty();
}

void AMazeActor::BuildMazeHISM(FMazeData* InMaze)
{
    if (!InMaze)
    {
        return;
    }

    if (!FloorHISM->GetStaticMesh() || !WallHISM->GetStaticMesh() || !GateHISM->GetStaticMesh())
    {
        UE_LOG(LogTemp, Error, TEXT("HISM 컴포넌트에 스태틱 메시가 없습니다."));
        return;
    }

    ClearHISMInstances();

    float CellSize = CellHalfSize * 2.0f;
    FVector StartLocation = FVector(Height * CellHalfSize, -Width * CellHalfSize, 0.0f)
        + FVector((Height % 2) * -CellHalfSize, (Width % 2) * CellHalfSize, 0.0f);

    // x, y는 MazeData의 좌표계 기준
    for (uint8 y = 0; y < Height; y++)
    {
        for (uint8 x = 0; x < Width; x++)
        {
            FCellData* Cell = InMaze->GetCell(x, y);
            if (!Cell)
            {
                continue;
            }

            FVector CellLocation = StartLocation + FVector(-y * CellSize, x * CellSize, 0.0f);

            MakeCellHISM(Cell, CellLocation);
        }
    }
}

void AMazeActor::ClearHISMInstances()
{
    if (FloorHISM)
    {
        FloorHISM->ClearInstances();
    }
    if (WallHISM)
    {
        WallHISM->ClearInstances();
    }
    if (GateHISM)
    {
        GateHISM->ClearInstances();
    }

    FlushPersistentDebugLines(GetWorld()); // 디버그 박스 지우기
}

void AMazeActor::MakeCellHISM(const FCellData* InCell, const FVector& InLocation)
{
    if (!InCell)
    {
        return;
    }

    // 바닥 추가
    FloorHISM->AddInstance(FTransform(FRotator::ZeroRotator, InLocation));

    // 벽 추가
    FVector Location;
    FRotator Rotation;

    Location = InLocation + FVector::ForwardVector * CellHalfSize;
    Rotation = FRotator::ZeroRotator;
    WallHISM->AddInstance(FTransform(Rotation, Location));
    if (InCell->IsWall(EDirectionType::North))
    {
        GateHISM->AddInstance(FTransform(Rotation, Location));
    }

    Location = InLocation + FVector::LeftVector * CellHalfSize;
    Rotation = FRotator(0.0f, -90.0f, 0.0f);
    WallHISM->AddInstance(FTransform(Rotation, Location));
    if (InCell->IsWall(EDirectionType::West))
    {
        GateHISM->AddInstance(FTransform(Rotation, Location));
    }

    if (InCell->X == Width - 1) // 동쪽 끝
    {
        Location = InLocation + FVector::RightVector * CellHalfSize;
        Rotation = FRotator(0.0f, 90.0f, 0.0f);
        WallHISM->AddInstance(FTransform(Rotation, Location));
        GateHISM->AddInstance(FTransform(Rotation, Location));
    }
    if (InCell->Y == Height - 1) // 남쪽 끝
    {
        Location = InLocation + FVector::BackwardVector * CellHalfSize;
        Rotation = FRotator(0.0f, 180.0f, 0.0f);
        WallHISM->AddInstance(FTransform(Rotation, Location));
        GateHISM->AddInstance(FTransform(Rotation, Location));
    }

#if WITH_EDITOR
    DrawDebugBox(
        GetWorld(),
        InLocation,
        FVector(CellHalfSize, CellHalfSize, 400.0f),
        FColor::Green,
        true,
        -1.0f,
        0,
        3.0f
    );
#endif
}
