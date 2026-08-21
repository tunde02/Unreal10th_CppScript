// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/ItemDataAsset.h"
#include "InventoryCommandTypes.generated.h"

UENUM(BlueprintType)
enum class EInventoryCommandType : uint8
{
    None,
    Add,
    Move,
    Drop,
    Use,
    Clear,
    Money
};

USTRUCT(BlueprintType)
struct UNREAL10TH_CPPSCRIPT_API FInventoryCommand
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    EInventoryCommandType Type = EInventoryCommandType::None;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    int32 Count = 1;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    TObjectPtr<const UItemDataAsset> ItemData = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    int32 SourceIndex = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    int32 TargetIndex = 0;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    FVector DropLocation = FVector::ZeroVector;

public:
    // Add 커맨드를 생성해주는 함수
    static FInventoryCommand MakeAddCommand(const UItemDataAsset* InItemData, int32 InCount = 1)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Add;
        Command.ItemData = InItemData;
        Command.Count = InCount;

        return Command;
    }

    static FInventoryCommand MakeMoveCommand(int32 InSourceIndex, int32 InTargetIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Move;
        Command.SourceIndex = InSourceIndex;
        Command.TargetIndex = InTargetIndex;

        return Command;
    }

    static FInventoryCommand MakeDropCommand(int32 InSlotIndex, const FVector& InDropLocation)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Drop;
        Command.SourceIndex = InSlotIndex;
        Command.DropLocation = InDropLocation;

        return Command;
    }

    static FInventoryCommand MakeUseCommand(int32 InSlotIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Use;
        Command.SourceIndex = InSlotIndex;

        return Command;
    }

    static FInventoryCommand MakeClearCommand(int32 InSlotIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Clear;
        Command.TargetIndex = InSlotIndex;

        return Command;
    }
    static FInventoryCommand MakeMoneyCommand(int32 InMoneyDiff)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Money;
        Command.Count = InMoneyDiff;

        return Command;
    }

};

USTRUCT(BlueprintType)
struct UNREAL10TH_CPPSCRIPT_API FInventoryCommandResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    bool bSuccess = false;

    UPROPERTY(BlueprintReadOnly)
    int32 RemainingCount = 0;
};
