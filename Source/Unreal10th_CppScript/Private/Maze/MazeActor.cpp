// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeActor.h"
#include "Maze/CellActor.h"
#include "Maze/MazeData.h"
#include "Maze/CellData.h"

AMazeActor::AMazeActor()
{
    PrimaryActorTick.bCanEverTick = false;

}

void AMazeActor::BeginPlay()
{
    Super::BeginPlay();
}

void AMazeActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMazeActor::GenerateMaze()
{
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
    MazeData.MakeMaze(Width, Height, Seed);

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
}

void AMazeActor::RemoveMaze()
{
    UE_LOG(LogTemp, Log, TEXT("Remove Maze."));

    for (auto& Cell : CellActors)
    {
        if (IsValid(Cell))
        {
            Cell->Destroy();
        }
    }

    CellActors.Empty();
}

