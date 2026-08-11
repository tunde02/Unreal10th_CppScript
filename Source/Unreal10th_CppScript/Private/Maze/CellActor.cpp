// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/CellActor.h"
#include "Components/ArrowComponent.h"

ACellActor::ACellActor()
{
    PrimaryActorTick.bCanEverTick = false;

    FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
    SetRootComponent(FloorMesh);
    FloorMesh->SetCollisionProfileName(TEXT("BlockAll"));

    UArrowComponent* Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("NorthArrow"));
    Arrow->SetupAttachment(GetRootComponent());
    Arrow->SetRelativeLocation(FVector(0.0f, 0.0f, 200.0f));

    SetCanBeDamaged(false);

    /*** Wall ***/
    UStaticMeshComponent* Wall;
    Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallNorth"));
    Wall->SetupAttachment(GetRootComponent());
    Wall->SetCollisionProfileName(TEXT("BlockAll"));
    Wall->SetRelativeLocationAndRotation(
        FVector::ForwardVector * (CellHalfSize - WallHalfThickness),
        FRotator(0.0f, 0.0f, 0.0f)
    );
    WallMeshes.Add(Wall);

    Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallEast"));
    Wall->SetupAttachment(GetRootComponent());
    Wall->SetCollisionProfileName(TEXT("BlockAll"));
    Wall->SetRelativeLocationAndRotation(
        FVector::RightVector * (CellHalfSize - WallHalfThickness),
        FRotator(0.0f, 90.0f, 0.0f)
    );
    WallMeshes.Add(Wall);

    Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallSouth"));
    Wall->SetupAttachment(GetRootComponent());
    Wall->SetCollisionProfileName(TEXT("BlockAll"));
    Wall->SetRelativeLocationAndRotation(
        FVector::BackwardVector * (CellHalfSize - WallHalfThickness),
        FRotator(0.0f, 180.0f, 0.0f)
    );
    WallMeshes.Add(Wall);

    Wall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallWest"));
    Wall->SetupAttachment(GetRootComponent());
    Wall->SetCollisionProfileName(TEXT("BlockAll"));
    Wall->SetRelativeLocationAndRotation(
        FVector::LeftVector * (CellHalfSize - WallHalfThickness),
        FRotator(0.0f, 270.0f, 0.0f)
    );
    WallMeshes.Add(Wall);

    /*** Gate ***/
    UStaticMeshComponent* Gate;
    Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateNorth"));
    Gate->SetupAttachment(GetRootComponent());
    Gate->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Gate->SetRelativeLocationAndRotation(
        FVector::ForwardVector * (CellHalfSize - WallHalfThickness),
        FRotator(0.0f, 0.0f, 0.0f)
    );
    GateMeshes.Add(Gate);

    Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateEast"));
    Gate->SetupAttachment(GetRootComponent());
    Gate->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Gate->SetRelativeLocationAndRotation(
        FVector::RightVector * (CellHalfSize - WallHalfThickness),
        FRotator(0.0f, 90.0f, 0.0f)
    );
    GateMeshes.Add(Gate);

    Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateSouth"));
    Gate->SetupAttachment(GetRootComponent());
    Gate->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Gate->SetRelativeLocationAndRotation(
        FVector::BackwardVector * (CellHalfSize - WallHalfThickness),
        FRotator(0.0f, 180.0f, 0.0f)
    );
    GateMeshes.Add(Gate);

    Gate = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateWest"));
    Gate->SetupAttachment(GetRootComponent());
    Gate->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    Gate->SetRelativeLocationAndRotation(
        FVector::LeftVector * (CellHalfSize - WallHalfThickness),
        FRotator(0.0f, 270.0f, 0.0f)
    );
    GateMeshes.Add(Gate);
}

void ACellActor::InitializeCell(FCellData* InCellData)
{
    if (!InCellData)
    {
        return;
    }

    Path = static_cast<int32>(InCellData->Path);
    OpenGate();
}

void ACellActor::BeginPlay()
{
    Super::BeginPlay();
}

void ACellActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACellActor::TestPath()
{
    UE_LOG(LogTemp, Log, TEXT("TestPath"));
    OpenGate();
}

void ACellActor::OpenGate()
{
    constexpr int DirectionCount = 4;
    for (int i = 0; i < DirectionCount; i++)
    {
        EDirectionType Direction = static_cast<EDirectionType>(1 << i);
        if (IsPath(Direction))
        {
            GateMeshes[i]->SetVisibility(false);
            GateMeshes[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        else
        {
            GateMeshes[i]->SetVisibility(true);
            GateMeshes[i]->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
        }
    }
}

bool ACellActor::IsPath(EDirectionType InDirection)
{
    return (static_cast<EDirectionType>(Path) & InDirection) != EDirectionType::None;
}
