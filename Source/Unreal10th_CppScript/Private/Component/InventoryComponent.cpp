// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddMoney(int32 InIncome)
{
    Money += FMath::Max(0, InIncome);
}

void UInventoryComponent::AddItem(UItemDataAsset* InItemData, int32 InCount)
{
    if (!InItemData || InCount <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.AddItem()] : Invalid ItemData OR Invalid Item Count."));
        return;
    }

    int32 SlotIndex = FindSlotWithItem(InItemData);
    if (SlotIndex != InventoryFail)
    {
        // TODO: ItemData.Count + InCount > ItemData.MaxStackCount일 경우
        // 1. 아이템 뭉치에서 일정량만 획득하고, 남은 건 그대로 있다
        // 2. 아이템 뭉치에서 일정량만 획득하고, 남은 건 없어진다
        UpdateSlotCount(SlotIndex, InCount);
        return;
    }

    SlotIndex = FindEmptySlot();
    if (SlotIndex != InventoryFail)
    {
        SetSlot(SlotIndex, InItemData, InCount);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.AddItem()] : 비어 있는 슬롯이 없어 %s를 획득할 수 없습니다."),
           *InItemData->DisplayName.ToString());
}

void UInventoryComponent::UseItem(int32 InSlotIndex)
{
    if (InSlotIndex < 0 || InSlotIndex >= InventorySize)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.UseItem()] : Invalid SlotIndex (%d)."), InSlotIndex);
        return;
    }

    FInvenSlot* InvenSlot = GetSlot(InSlotIndex);
    if (!InvenSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.UseItem()] : InvenSlot nullptr"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.UseItem()] : %s를 사용합니다."),
           *InvenSlot->ItemData->DisplayName.ToString());
    UpdateSlotCount(InSlotIndex, -1);
}

FInvenSlot* UInventoryComponent::GetSlot(int InSlotIndex)
{
    if (InSlotIndex < 0 || InSlotIndex >= InventorySize)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.GetSlot()] : Invalid SlotIndex"));
        return nullptr;
    }

    return &Slots[InSlotIndex];
}

FInvenSlot* UInventoryComponent::GetTempSlot()
{
    return nullptr;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::UpdateSlotCount(int32 InSlotIndex, int32 InDeltaCount)
{
    FInvenSlot* InvenSlot = GetSlot(InSlotIndex);
    if (!InvenSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.UpdateSlotCount()] : InvenSlot nullptr"));
        return;
    }

    InvenSlot->SetCount(InvenSlot->GetCount() + InDeltaCount);

    if (InvenSlot->IsEmpty())
    {
        UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.UpdateSlotcount()] : %s를 모두 사용하여 슬롯을 비웁니다."),
               *InvenSlot->ItemData->DisplayName.ToString());
        ClearSlot(InSlotIndex);
    }
}

void UInventoryComponent::SetSlot(int32 InSlotIndex, UItemDataAsset* InItemData, int32 InCount)
{
    if (InSlotIndex < 0 || InSlotIndex >= InventorySize
        || !InItemData
        || InCount <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.SetSlot()] : Invalid SlotIndex OR ItemData OR Count"));
        return;
    }

    FInvenSlot* InvenSlot = GetSlot(InSlotIndex);
    if (!InvenSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.SetSlot()] : InvenSlot nullptr"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.SetSlot()] : %d번째 슬롯에 %s아이템을 %d개 설정합니다"),
           InSlotIndex, *InvenSlot->ItemData->DisplayName.ToString(), InCount);
    InvenSlot->ItemData = InItemData;
    InvenSlot->SetCount(InCount);
}

void UInventoryComponent::ClearSlot(int32 InSlotIndex)
{
    if (InSlotIndex < 0 || InSlotIndex >= InventorySize)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.ClearSlot()] : Invalid SlotIndex"));
        return;
    }

    FInvenSlot* InvenSlot = GetSlot(InSlotIndex);
    if (!InvenSlot)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.ClearSlot()] : InvenSlot nullptr"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.SetSlot()] : %d번째 슬롯을 비웁니다. (%s %d개)"),
           InSlotIndex, *InvenSlot->ItemData->DisplayName.ToString(), InvenSlot->GetCount());
    InvenSlot->Clear();
}

int32 UInventoryComponent::FindSlotWithItem(const UItemDataAsset* InItemData, int32 InStartIndex)
{
    if (!InItemData
        || InStartIndex < 0 || InStartIndex >= InventorySize)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.FindSlotWithItem()] : Invalid ItemData OR StartIndex"));
        return InventoryFail;
    }

    for (int i = InStartIndex; i < InventorySize; i++)
    {
        if (Slots[i].ItemData == InItemData)
        {
            return i;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.FindSlotWithItem()] : %s 슬롯을 찾지 못했음"),
           *InItemData->DisplayName.ToString());
    return InventoryFail;
}

int32 UInventoryComponent::FindEmptySlot()
{
    for (int i = 0; i < InventorySize; i++)
    {
        if (Slots[i].IsEmpty())
        {
            return i;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.FindEmptySlot()] : 빈 슬롯을 찾지 못했음"));
    return InventoryFail;
}

