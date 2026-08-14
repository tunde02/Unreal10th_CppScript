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
    Use
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
    FVector2D ScreenPosition = FVector2D::ZeroVector;

    UPROPERTY(BlueprintReadWrite, Category = "Inventory|Command")
    FTransform DropTransform = FTransform::Identity;

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

    static FInventoryCommand MakeMoveCommand(const UItemDataAsset* InItemData, int32 InCount, int32 InSourceIndex, int32 InTargetIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Move;
        Command.ItemData = InItemData;
        Command.Count = InCount;
        Command.SourceIndex = InSourceIndex;
        Command.TargetIndex = InTargetIndex;

        return Command;
    }

    static FInventoryCommand MakeDropCommand(const UItemDataAsset* InItemData, int32 InCount, int32 InSlotIndex, FTransform InTransform)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Drop;
        Command.ItemData = InItemData;
        Command.Count = InCount;
        Command.SourceIndex = InSlotIndex;
        Command.DropTransform = InTransform;

        return Command;
    }

    static FInventoryCommand MakeUseCommand(const UItemDataAsset* InItemData, int32 InCount, int32 InSlotIndex)
    {
        FInventoryCommand Command;
        Command.Type = EInventoryCommandType::Use;
        Command.ItemData = InItemData;
        Command.Count = InCount;
        Command.SourceIndex = InSlotIndex;

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
