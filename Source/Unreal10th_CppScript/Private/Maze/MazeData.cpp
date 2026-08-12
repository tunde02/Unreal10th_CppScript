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

void FMazeData::MakeMaze(uint8 InWidth, uint8 InHeight, int32 InSeed, EMazeAlgorithmType InMazeAlgorithmType)
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

    switch (InMazeAlgorithmType)
    {
        case EMazeAlgorithmType::Eller:
            ExecuteEllerAlgorithm();
            break;
        case EMazeAlgorithmType::Wilson:
            ExecuteWilsonAlgorithm();
            break;
        default:
            break;
    }
}

void FMazeData::ClearMaze()
{
    Width = 0;
    Height = 0;
    Cells.Empty();
}

void FMazeData::ExecuteEllerAlgorithm()
{
    TArray<FEllerCellData> EllerCells;
    EllerCells.SetNum(Width * Height);

    int32 CurrentCellSet = 1;
    for (uint8 y = 0; y < Height; y++)
    {
        for (uint8 x = 0; x < Width; x++)
        {
            uint16 Index = LocationToIndex(x, y);
            FEllerCellData& Cell = EllerCells[Index];
            Cell.X = x;
            Cell.Y = y;
            Cell.Path = EDirectionType::None;
            Cell.SetID = CurrentCellSet++;
        }
    }

    //TArray<FCellData*> CellRow;
    //CellRow.Reserve(Width);

    //for (uint8 x = 0; x < Width; x++)
    //{
    //    FCellData* Cell = GetCell(x, 0);
    //    Cell->X = x;
    //    Cell->Y = 0;
    //    Cell->SetID = CurrentCellSet++;
    //    CellRow.Add(Cell);
    //}

    //for (int y = 0; y < 4; y++)
    for (int y = 0; y < Height - 1; y++)
    {
        // Join
        TArray<int> Joins = SelectRandomNumbers(1, Width - 1, RandomStream.RandRange(0, Width - 1));
        Joins.Sort();
        for (int i = 0; i < Joins.Num(); i++)
        {
            FEllerCellData& Cell_1 = EllerCells[LocationToIndex(Joins[i] - 1, y)];
            FEllerCellData& Cell_2 = EllerCells[LocationToIndex(Joins[i], y)];

            if (Cell_1.SetID != Cell_2.SetID)
            {
                int32 MinCellSet = FMath::Min(Cell_1.SetID, Cell_2.SetID);
                int32 CellSet_1 = Cell_1.SetID;
                int32 CellSet_2 = Cell_2.SetID;
                for (int x = 0; x < Width; x++)
                {
                    FEllerCellData& Cell = EllerCells[LocationToIndex(x, y)];
                    if (Cell.SetID == CellSet_1 || Cell.SetID == CellSet_2)
                    {
                        Cell.SetID = MinCellSet;
                    }
                }
                //Cell_1.SetID = MinCellSet;
                //Cell_2.SetID = MinCellSet;
                ConnectCells(&Cell_1, &Cell_2);
            }
        }

        //ExecuteEllerAlgorithmSingleRow(CellRow, y);

        // Vertical
        TMap<int32, TArray<int>> CellSetMap;
        for (int x = 0; x < Width; x++)
        {
            FEllerCellData& Cell = EllerCells[LocationToIndex(x, y)];
            CellSetMap.FindOrAdd(Cell.SetID).Add(x);
        }

        for (const auto& [SetID, CellIndexs] : CellSetMap)
        {
            TArray<int> Verticals = SelectRandomNumbers(0, CellIndexs.Num() - 1, RandomStream.RandRange(1, CellIndexs.Num()));
            for (int i = 0; i < Verticals.Num(); i++)
            {
                int x = CellIndexs[Verticals[i]];
                FEllerCellData& Cell = EllerCells[LocationToIndex(x, y)];
                FEllerCellData& VerticalCell = EllerCells[LocationToIndex(x, y + 1)];

                VerticalCell.SetID = Cell.SetID;
                ConnectCells(&Cell, &VerticalCell);
            }
        }

        // Fill Cell Sets
        CurrentCellSet = 0;
        for (int x = 0; x < Width; x++)
        {
            FEllerCellData& Cell = EllerCells[LocationToIndex(x, y + 1)];
            CurrentCellSet = FMath::Max(CurrentCellSet, Cell.SetID);
        }
        CurrentCellSet++;

        for (int x = 0; x < Width; x++)
        {
            FEllerCellData& Cell = EllerCells[LocationToIndex(x, y + 1)];
            if (Cell.SetID == 0)
            {
                Cell.SetID = CurrentCellSet++;
            }
        }
    }

    // Last Row
    for (int x = 1; x < Width; x++)
    {
        FEllerCellData& Cell_1 = EllerCells[LocationToIndex(x - 1, Height - 1)];
        FEllerCellData& Cell_2 = EllerCells[LocationToIndex(x, Height - 1)];

        if (Cell_1.SetID != Cell_2.SetID)
        {
            int32 TargetCellSet = Cell_2.SetID;
            for (int i = 0; i < Width; i++)
            {
                FEllerCellData& Cell = EllerCells[LocationToIndex(i, Height - 1)];
                if (Cell.SetID == TargetCellSet)
                {
                    Cell.SetID = Cell_1.SetID;
                }
            }
            //Cell_2->SetID = Cell_1->SetID;
            ConnectCells(&Cell_1, &Cell_2);
        }
    }

    // 최종 생성 결과를 FMazeData의 Cells 배열로 복사
    CopyToCells(EllerCells);
}

TArray<FEllerCellData*> FMazeData::ExecuteEllerAlgorithmSingleRow(TArray<FEllerCellData*>& InCellRow, int InRowNum)
{
    TArray<FEllerCellData*> CellRow;
    CellRow.Reserve(Width);
    /*
    // Vertical
    TMap<int32, TArray<int>> CellSetMap;
    for (int x = 0; x < Width; x++)
    {
        FEllerCellData* Cell = EllerCells[LocationToIndex(x, InRowNum)];
        CellSetMap.FindOrAdd(Cell->SetID).Add(x);
    }

    for (const auto& [SetID, CellIndexs] : CellSetMap)
    {
        TArray<int> Verticals = SelectRandomNumbers(0, CellIndexs.Num() - 1, RandomStream.RandRange(1, CellIndexs.Num()));
        for (int i = 0; i < Verticals.Num(); i++)
        {
            int x = CellIndexs[Verticals[i]];
            FEllerCellData* Cell = EllerCells[LocationToIndex(x, InRowNum)];
            FEllerCellData* VerticalCell = EllerCells[LocationToIndex(x, InRowNum + 1)];

            VerticalCell->SetID = Cell->SetID;
            ConnectCells(Cell, VerticalCell);
        }
    }

    // Fill Cell Sets
    int32 CurrentCellSet = 0;
    for (int x = 0; x < Width; x++)
    {
        FEllerCellData* Cell = EllerCells[LocationToIndex(x, InRowNum + 1)];
        CurrentCellSet = FMath::Max(CurrentCellSet, Cell->SetID);
    }
    CurrentCellSet++;

    for (int x = 0; x < Width; x++)
    {
        FEllerCellData* Cell = EllerCells[LocationToIndex(x, InRowNum + 1)];
        if (Cell->SetID == 0)
        {
            Cell->SetID = CurrentCellSet++;
        }
    }

    return CellRow;
    */

    return CellRow;
}

void FMazeData::ExecuteWilsonAlgorithm()
{
    // 임시 배열
    TArray<FWilsonCellData> WilsonCells;
    WilsonCells.SetNum(Width * Height);

    // 기본 초기화
    TArray<FWilsonCellData*> NotInMazeCells;
    NotInMazeCells.Reserve(Width * Height);

    for (int32 y = 0; y < Height; y++)
    {
        for (int32 x = 0; x < Width; x++)
        {
            uint16 Index = LocationToIndex(x, y);
            FWilsonCellData& Cell = WilsonCells[Index];
            Cell.X = x;
            Cell.Y = y;
            Cell.Path = EDirectionType::None;
            Cell.bInMaze = false;
            Cell.NextCell = nullptr;
            NotInMazeCells.Add(&Cell);
        }
    }

    ShuffleArray(NotInMazeCells);

    FWilsonCellData* InitCell = NotInMazeCells.Pop();
    InitCell->bInMaze = true;

    while (NotInMazeCells.Num() > 0)
    {
        FWilsonCellData* StartCell = NotInMazeCells.Pop();

        // 이미 미로에 포함된 셀은 스킵
        if (StartCell->bInMaze)
        {
            continue;
        }

        FWilsonCellData* CurrentCell = StartCell;

        // 랜덤 워크 진행
        do
        {
            FIntPoint NeighborLocation;
            do
            {
                int32 DirIdx = RandomStream.RandRange(0, DirectionCount - 1);
                NeighborLocation = CurrentCell->GetLocation() + Direction[DirIdx];
            } while (!IsValidLocation(NeighborLocation.X, NeighborLocation.Y));

            FWilsonCellData* NeighborCell = &WilsonCells[LocationToIndex(NeighborLocation.X, NeighborLocation.Y)];
            CurrentCell->NextCell = NeighborCell;
            CurrentCell = NeighborCell; // 이웃 셀 기준으로 계속 진행
        } while (!CurrentCell->bInMaze);

        // 경로를 따라 미로에 포함시키기
        FWilsonCellData* Path = StartCell;
        while (Path != CurrentCell) // 미로에 포함되어 있는 셀에 도달할 때까지 반복
        {
            Path->bInMaze = true;
            ConnectCells(Path, Path->NextCell);
            Path = Path->NextCell;
        }
    }

    // 생성 결과를 FMazeData의 Cells로 전달
    CopyToCells(WilsonCells);
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

TArray<int> FMazeData::SelectRandomNumbers(int InFrom, int InTo, int InCount)
{
    TArray<int> Numbers;
    Numbers.Reserve(InTo - InFrom + 1);
    for (int i = InFrom; i <= InTo; i++)
    {
        Numbers.Add(i);
    }

    for (int i = Numbers.Num() - 1; i > 0; i--)
    {
        int32 Index = RandomStream.FRandRange(0, i);
        Numbers.Swap(i, Index);
    }

    Numbers.SetNum(InCount);

    return Numbers;
}
