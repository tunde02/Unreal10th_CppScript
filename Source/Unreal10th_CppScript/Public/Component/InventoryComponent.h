// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/Item/ItemDataAsset.h"
#include "InventoryComponent.generated.h"

class UItemDataAsset;

USTRUCT(BlueprintType)
struct FInvenSlot
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot")
    TObjectPtr<UItemDataAsset> ItemData;

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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREAL10TH_CPPSCRIPT_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    void AddMoney(int32 InIncome);
    void AddItem(UItemDataAsset* InItemData, int32 InCount);
    void UseItem(int32 InSlotIndex);

    /* Getter **************************************************/
    int32 GetMoney() const { return Money; }
    FInvenSlot* GetSlot(int InSlotIndex);

    // 임시 슬롯을 반환하는 함수 (드래그앤 드롭에 사용)
    FInvenSlot* GetTempSlot();
    /***********************************************************/

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void UpdateSlotCount(int32 InSlotIndex, int32 InDeltaCount);
    void SetSlot(int32 InSlotIndex, UItemDataAsset* InItemData, int32 InCount);
    void ClearSlot(int32 InSlotIndex);

    inline bool IsValidIndex(int32 InSlotIndex) const { return 0 < InSlotIndex && InSlotIndex < InventorySize; }

private:
    // 같은 종류의 아이템이 있는 슬롯을 찾는 함수
    int32 FindSlotWithItem(const UItemDataAsset* InItemData, int32 InStartIndex = 0);

    // 비어 있는 슬롯을 찾는 함수
    int32 FindEmptySlot();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Money")
    int32 Money = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory|Slot")
    TArray<FInvenSlot> Slots;

private:
    static constexpr int32 InventorySize = 10;
    static constexpr int32 InventoryFail = -1;

};
