// Fill out your copyright notice in the Description page of Project Settings.


#include "Maze/MazeData.h"
#include "Maze/CellData.h"

const FIntPoint FMazeData::Direction[DirectionCount] =
{
    FIntPoint(0, 1), FIntPoint(1, 0), FIntPoint(0, -1), FIntPoint(-1, 0)
};

FMazeData::~FMazeData()
{
    ClearMaze();
}

void FMazeData::MakeMaze(uint8 InWidth, uint8 InHeight, int32 InSeed)
{
    ClearMaze();

    Width = InWidth;
    Height = InHeight;

    if (RandomSeed == InSeed)
    {
        // 시드값을 랜덤하게 설정
        RandomStream.GenerateNewSeed();
    }
    else
    {
        // 시드값을 정해진 값으로 설정
        RandomStream.Initialize(InSeed);
    }

    Cells.SetNum(Width * Height);

    ExecuteWilsonAlgorithm();
}

void FMazeData::ClearMaze()
{
    Width = 0;
    Height = 0;
    Cells.Empty();
}

void FMazeData::ExecuteWilsonAlgorithm()
{
    // 기본 초기화
    TArray<FCellData*> NotInMazeCells;
    NotInMazeCells.Reserve(Width * Height);

    for (int32 y = 0; y < Height; y++)
    {
        for (int32 x = 0; x < Width; x++)
        {
            FCellData* Cell = GetCell(x, y);
            Cell->X = x;
            Cell->Y = y;
            NotInMazeCells.Add(Cell);
        }
    }

    ShuffleArray(NotInMazeCells);

    FCellData* InitCell = NotInMazeCells.Pop();
    InitCell->bInMaze = true;

    while (NotInMazeCells.Num() > 0)
    {
        FCellData* StartCell = NotInMazeCells.Pop();

        // 이미 미로에 포함된 셀은 스킵
        if (StartCell->bInMaze)
        {
            continue;
        }

        FCellData* CurrentCell = StartCell;

        // 랜덤 워크 진행
        do
        {
            FCellData* NeighborCell = GetRandomNeighborCell(*CurrentCell);
            CurrentCell->NextCell = NeighborCell;
            CurrentCell = NeighborCell; // 이웃 셀 기준으로 계속 진행
        } while (!CurrentCell->bInMaze);

        // 경로를 따라 미로에 포함시키기
        FCellData* Path = StartCell;
        while (Path != CurrentCell) // 미로에 포함되어 있는 셀에 도달할 때까지 반복
        {
            Path->bInMaze = true;
            ConnectCells(Path, Path->NextCell);
            Path = Path->NextCell;
        }
    }
}

FCellData* FMazeData::GetCell(uint8 InX, uint8 InY)
{
    if (!IsValidLocation(InX, InY))
    {
        return nullptr;
    }

    return &(Cells[LocationToIndex(InX, InY)]);
}

void FMazeData::ConnectCells(FCellData* InFrom, FCellData* InTo)
{
    // From과 To가 모두 있어야 하고, 서로 다른 셀이어야 한다
    if (!InFrom || !InTo || InFrom == InTo)
    {
        return;
    }

    if (InFrom->X < InTo->X)
    {
        // From의 동쪽에 To가 있다
        InFrom->AddPath(EDirectionType::East);
        InTo->AddPath(EDirectionType::West);
    }
    else if (InFrom->X > InTo->X)
    {
        // From의 서쪽에 To가 있다
        InFrom->AddPath(EDirectionType::West);
        InTo->AddPath(EDirectionType::East);
    }
    else if (InFrom->Y < InTo->Y)
    {
        // From의 남쪽에 To가 있다
        InFrom->AddPath(EDirectionType::South);
        InTo->AddPath(EDirectionType::North);
    }
    else if (InFrom->Y > InTo->Y)
    {
        // From의 북쪽에 To가 있다
        InFrom->AddPath(EDirectionType::North);
        InTo->AddPath(EDirectionType::South);
    }
}

FCellData* FMazeData::GetRandomNeighborCell(const FCellData& InCell)
{
    FIntPoint NeighborLocation;

    do
    {
        int32 Index = RandomStream.RandRange(0, DirectionCount - 1);
        NeighborLocation = InCell.GetLocation() + Direction[Index];
    } while (!IsValidLocation(NeighborLocation.X, NeighborLocation.Y));

    return GetCell(static_cast<uint8>(NeighborLocation.X), static_cast<uint8>(NeighborLocation.Y));
}

void FMazeData::ShuffleArray(TArray<FCellData*>& InOutArray)
{
    for (int i = InOutArray.Num() - 1; i > 0; i--)
    {
        int32 Index = RandomStream.FRandRange(0, i);
        InOutArray.Swap(i, Index);
    }
}
