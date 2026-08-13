// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Maze/CellData.h"

UENUM(BlueprintType)
enum class EMazeAlgorithmType : uint8
{
    Wilson,
    Eller
};

struct FCellData;

class UNREAL10TH_CPPSCRIPT_API FMazeData
{
public:
    ~FMazeData(); // FMazeData가 상속이 된다면 반드시 가상 소멸자를 사용해야 한다

    void MakeMaze(uint8 InWidth, uint8 InHeight, int32 InSeed = RandomSeed, EMazeAlgorithmType InMazeAlgorithmType = EMazeAlgorithmType::Wilson);
    void ClearMaze();

    FCellData* GetCell(uint8 InX, uint8 InY);

private:
    void ExecuteEllerAlgorithm();
    void ExecuteWilsonAlgorithm();

    // From셀과 To셀 사이의 문을 제거하는 함수
    void ConnectCells(FCellData* InFrom, FCellData* InTo);

    // InCell 주변의 셀 중 랜덤하게 하나 리턴하는 함수
    FCellData* GetRandomNeighborCell(const FCellData& InCell);

    template<typename T>
    void ShuffleArray(TArray<T*>& InOutArray)
    {
        for (int i = InOutArray.Num() - 1; i > 0; i--)
        {
            int32 Index = RandomStream.RandRange(0, i);
            InOutArray.Swap(i, Index);
        }
    }

    // 작업용 셀 배열의 생성 결과를 FMazeData의 연속 메모리 Cells 배열로 복사하는 헬퍼 함수
    template<typename TCellType>
    void CopyToCells(const TArray<TCellType>& InSrcCells)
    {
        int32 Count = FMath::Min(Cells.Num(), InSrcCells.Num());
        for (int32 i = 0; i < Count; i++)
        {
            Cells[i].X = InSrcCells[i].X;
            Cells[i].Y = InSrcCells[i].Y;
            Cells[i].Path = InSrcCells[i].Path;
        }
    }

    // 위치를 인덱스로 변경하는 함수
    inline uint16 LocationToIndex(uint8 InX, uint8 InY) const { return InX + InY * Width; }

    inline bool IsValidLocation(uint8 InX, uint8 InY) const { return InX < Width && InY < Height; }

    TArray<int> SelectRandomNumbers(int InFrom, int InTo, int InCount);

public:
    static constexpr int32 RandomSeed = -1;

private:
    uint8 Width = 3;
    uint8 Height = 3;

    static constexpr uint8 MinimumSize = 3;
    static constexpr uint8 MaximumSize = 100;

    // 미로의 모든 칸이 들어 있는 배열
    // 1차원 배열로 만들어서 캐싱 효율 증가를 노림
    TArray<FCellData> Cells;

    // 랜덤한 수를 뽑기 위한 스트림
    FRandomStream RandomStream;

    static constexpr int DirectionCount = 4;

    // 이웃 방향의 상대 좌표 (북동남서)
    static const FIntPoint Direction[DirectionCount];
};
