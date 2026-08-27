// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component/InventoryCommandTypes.h"
#include "Data/Item/ItemDataAsset.h"
#include "InventoryComponent.generated.h"

class UItemDataAsset;
class UTemporarySlotWidget;

USTRUCT(BlueprintType)
struct FInvenSlot
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
    TObjectPtr<const UItemDataAsset> ItemData;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
    int32 Count = 0;

public:
    inline bool IsEmpty() const { return ItemData == nullptr; }
    inline bool IsFull() const { return ItemData && (Count >= ItemData->MaxStackCount); }
    inline void Clear()
    {
        ItemData = nullptr;
        Count = 0;
    }
    inline int32 GetRemainingCount() const { return ItemData ? ItemData->MaxStackCount - Count : 0; }
    inline int32 GetCount() const { return Count; }
    inline  void SetCount(int32 InCount)
    {
        if (ItemData && InCount > 0)
        {
            Count = FMath::Clamp(InCount, 0, ItemData->MaxStackCount);
        }
        else
        {
            Clear();
        }
    }
};

DECLARE_DELEGATE_OneParam(FOnSlotChanged, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMoneyChanged, int32);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREAL10TH_CPPSCRIPT_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    // 커맨드 실행용 함수
    UFUNCTION(BlueprintCallable, Category = "Inventory|Command")
    bool ExecuteCommand(const FInventoryCommand& Command, FInventoryCommandResult& OutResult);

    /* Getter **************************************************/
    inline int32 GetMoney() const { return Money; }
    FInvenSlot* GetSlot(int InSlotIndex);
    inline int32 GetSize() const { return InventorySize; }
    inline int32 GetTempSlotIndex() const { return TempSlotIndex; }

    // 임시 슬롯을 반환하는 함수 (드래그앤 드롭에 사용)
    FInvenSlot* GetTempSlot();

    // 임시 슬롯의 위젯 클래스를 반환하는 함수
    inline TSubclassOf<UTemporarySlotWidget> GetTemporarySlotWidgetClass() const { return TemporarySlotWidgetClass; }
    /***********************************************************/

protected:
    UFUNCTION(BlueprintCallable)
    void AddMoney(int32 InIncome);

    UFUNCTION(BlueprintCallable)
    int32 AddItem(const UItemDataAsset* InItemData, int32 InCount);

    void UseItem(int32 InSlotIndex);
    void EquipItem(int32 InSlotIndex);
    void SetSlot(int32 InSlotIndex, const UItemDataAsset* InItemData, int32 InCount);
    void UpdateSlotCount(int32 InSlotIndex, int32 InDeltaCount);
    void ClearSlot(int32 InSlotIndex);

    inline bool IsValidIndex(int32 InSlotIndex) const { return 0 <= InSlotIndex && InSlotIndex <= InventorySize; }

    bool HandleAddCommand(const UItemDataAsset* InItemData, int32 InCount, FInventoryCommandResult& OutResult);
    bool HandleMoveCommand(int32 InSourceIndex, int32 InTargetIndex, FInventoryCommandResult& OutResult);
    bool HandleDropCommand(int32 InSlotIndex, const FVector& InDropLocation, FInventoryCommandResult& OutResult);
    bool HandleUseCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult);
    bool HandleClearCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult);
    bool HandleMoneyCommand(int32 InMoneyDiff, FInventoryCommandResult& OutResult);
    bool HandleEquipCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult);

    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    // 같은 종류의 아이템이 있는 슬롯을 찾는 함수
    int32 FindSlotWithItem(const UItemDataAsset* InItemData, int32 InStartIndex = 0);

    // 비어 있는 슬롯을 찾는 함수
    int32 FindEmptySlot();

public:
    FOnSlotChanged OnSlotChanged;
    FOnMoneyChanged OnMoneyChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Money")
    int32 Money = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
    TArray<FInvenSlot> Slots;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
    TSubclassOf<UTemporarySlotWidget> TemporarySlotWidgetClass;

private:
    static constexpr int32 InventorySize = 10;
    static constexpr int32 TempSlotIndex = InventorySize;
    static constexpr int32 InventoryFail = -1;

};
