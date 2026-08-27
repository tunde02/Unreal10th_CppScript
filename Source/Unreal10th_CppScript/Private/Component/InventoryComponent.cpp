// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"
#include "Framework/Subsystem/PickupFactorySubsystem.h"
#include "Data/Item/UsableItemDataAsset.h"
#include "Data/Item/WeaponDataAsset.h"

// DELETE ME?
#include "Interface/WeaponUserInterface.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    Slots.SetNum(InventorySize + 1);
}

bool UInventoryComponent::ExecuteCommand(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    switch (Command.Type)
    {
        case EInventoryCommandType::Add:
            HandleAddCommand(Command.ItemData, Command.Count, OutResult);
            break;
        case EInventoryCommandType::Move:
            HandleMoveCommand(Command.SourceIndex, Command.TargetIndex, OutResult);
            break;
        case EInventoryCommandType::Drop:
            HandleDropCommand(Command.SourceIndex, Command.DropLocation, OutResult);
            break;
        case EInventoryCommandType::Use:
            HandleUseCommand(Command.SourceIndex, OutResult);
            break;
        case EInventoryCommandType::Clear:
            HandleClearCommand(Command.TargetIndex, OutResult);
            break;
        case EInventoryCommandType::Money:
            HandleMoneyCommand(Command.Count, OutResult);
            break;
        case EInventoryCommandType::Equip:
            HandleEquipCommand(Command.SourceIndex, OutResult);
            break;
        case EInventoryCommandType::None:
        default:
            UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.ExecuteCommand()] : 알 수 없는 커맨드입니다."));
            break;
    }

    return OutResult.bSuccess;
}

FInvenSlot* UInventoryComponent::GetSlot(int InSlotIndex)
{
    check(IsValidIndex(InSlotIndex));

    return &Slots[InSlotIndex];
}

FInvenSlot* UInventoryComponent::GetTempSlot()
{
    return &Slots[TempSlotIndex];
}

void UInventoryComponent::AddMoney(int32 InIncome)
{
    Money += InIncome;
    OnMoneyChanged.Broadcast(Money);
}

int32 UInventoryComponent::AddItem(const UItemDataAsset* InItemData, int32 InCount)
{
    if (!InItemData)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.AddItem()] : InItemData가 nullptr입니다."));
        return InCount;
    }

    if (InCount <= 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[InventoryComponent.AddItem()] : InCount가 0이하입니다."));
        return InCount;
    }

    int32 RemainingCount = InCount;

    int32 StartIndex = 0;
    while (RemainingCount > 0)
    {
        // 같은 종류의 아이템이 들어 있는 슬롯을 찾아 추가
        int32 FoundIndex = FindSlotWithItem(InItemData, StartIndex);
        if (FoundIndex == InventoryFail)
        {
            break;
        }

        FInvenSlot& Slot = Slots[FoundIndex];
        int32 AmountToAdd = FMath::Min(Slot.GetRemainingCount(), RemainingCount);
        UpdateSlotCount(FoundIndex, AmountToAdd); // FoundIndex 슬롯에 채울 수 있는 만큼 채우기

        RemainingCount -= AmountToAdd; // 남은 개수 갱신
        StartIndex = FoundIndex + 1; // 새 시작 위치 갱신
    }

    while (RemainingCount > 0)
    {
        int32 EmptyIndex = FindEmptySlot();
        if (EmptyIndex == InventoryFail)
        {
            break;
        }

        FInvenSlot& Slot = Slots[EmptyIndex];
        int32 AmountToAdd = FMath::Min(InItemData->MaxStackCount, RemainingCount);
        SetSlot(EmptyIndex, InItemData, AmountToAdd); // EmptyIndex 슬롯에 아이템 설정

        RemainingCount -= AmountToAdd; // 남은 개수 갱신
    }

    // 남은 개수가 0을 초과하면 그만큼은 인벤토리에 못들어갔다는 의미
    return RemainingCount;
}

void UInventoryComponent::UseItem(int32 InSlotIndex)
{
    if (!IsValidIndex(InSlotIndex))
    {
        return;
    }

    FInvenSlot& Slot = Slots[InSlotIndex];

    if (Slot.IsEmpty())
    {
        return;
    }

    if (const UUsableItemDataAsset* Usable = Cast<const UUsableItemDataAsset>(Slot.ItemData))
    {
        if (Usable->ItemAction)
        {
            UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.HandleUseCommand()] : %s를 사용했습니다."), *Slot.ItemData->DisplayName.ToString());
            Usable->ItemAction->ExecuteAction_Implementation(GetOwner(), GetOwner());
            UpdateSlotCount(InSlotIndex, -1);
        }
    }
    /* DELETE ME?
    else if (const UWeaponDataAsset* Weapon = Cast<const UWeaponDataAsset>(Slot.ItemData))
    {
        if (Weapon->ItemAction)
        {
            UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.HandleUseCommand()] : %s를 장착했습니다."), *Slot.ItemData->DisplayName.ToString());
            Weapon->ItemAction->ExecuteAction_Implementation(GetOwner(), GetOwner());
            UpdateSlotCount(InSlotIndex, -1);
        }
    }
    */
}

void UInventoryComponent::EquipItem(int32 InSlotIndex)
{
    FInvenSlot* Slot = GetSlot(InSlotIndex);
    if (!Slot)
    {
        return;
    }

    if (const UWeaponDataAsset* WeaponData = Cast<const UWeaponDataAsset>(Slot->ItemData))
    {
        if (GetOwner()->Implements<UWeaponUserInterface>())
        {
            UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.HandleEquipCommand()] : %s를 사용했습니다."), *Slot->ItemData->DisplayName.ToString());
            IWeaponUserInterface::Execute_EquipWeapon(GetOwner(), WeaponData);
            UpdateSlotCount(InSlotIndex, -1);
        }
    }
}

void UInventoryComponent::SetSlot(int32 InSlotIndex, const UItemDataAsset* InItemData, int32 InCount)
{
    if (!IsValidIndex(InSlotIndex))
    {
        return;
    }

    FInvenSlot& Slot = Slots[InSlotIndex];
    Slot.ItemData = InItemData;
    Slot.SetCount(InCount);

    if (InItemData && !InItemData->IsLoaded())
    {
        InItemData->RequestDataLoad(
            FStreamableDelegate::CreateWeakLambda(
                this,
                [this, InSlotIndex]() {
                    OnSlotChanged.ExecuteIfBound(InSlotIndex);
                }
            )
        );
    }

    // 델리게이트 전담 함수
    OnSlotChanged.ExecuteIfBound(InSlotIndex);
}

void UInventoryComponent::UpdateSlotCount(int32 InSlotIndex, int32 InDeltaCount)
{
    if (!IsValidIndex(InSlotIndex))
    {
        return;
    }

    FInvenSlot& Slot = Slots[InSlotIndex];
    if (Slot.IsEmpty())
    {
        return;
    }

    int32 NewCount = Slot.GetCount() + InDeltaCount;
    SetSlot(InSlotIndex, Slot.ItemData, NewCount);
}

void UInventoryComponent::ClearSlot(int32 InSlotIndex)
{
    SetSlot(InSlotIndex, nullptr, 0);
}

bool UInventoryComponent::HandleAddCommand(const UItemDataAsset* InItemData, int32 InCount, FInventoryCommandResult& OutResult)
{
    int32 RemainingCount = AddItem(InItemData, InCount);

    if (RemainingCount > 0)
    {
        OutResult.bSuccess = false;
        OutResult.RemainingCount = RemainingCount;
    }
    else
    {
        OutResult.bSuccess = true;
        OutResult.RemainingCount = 0;
    }

    if (OutResult.bSuccess)
    {
        UE_LOG(LogTemp, Log, TEXT("[%s] 추가가 성공적으로 완료되었습니다."), *(InItemData->DisplayName.ToString()));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("%d개의 아이템이 남았습니다."), OutResult.RemainingCount);
    }

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleMoveCommand(int32 InSourceIndex, int32 InTargetIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(InSourceIndex) || !IsValidIndex(InTargetIndex))
    {
        return OutResult.bSuccess;
    }

    if (InSourceIndex == InTargetIndex)
    {
        OutResult.bSuccess = true;
        return OutResult.bSuccess;
    }

    FInvenSlot& SourceSlot = Slots[InSourceIndex];
    FInvenSlot& TargetSlot = Slots[InTargetIndex];

    if (SourceSlot.IsEmpty())
    {
        return OutResult.bSuccess;
    }

    // 1. 타겟 슬롯이 비어있다 -> 그대로 이동
    if (TargetSlot.IsEmpty())
    {
        SetSlot(InTargetIndex, SourceSlot.ItemData, SourceSlot.GetCount());
        ClearSlot(InSourceIndex);

        OutResult.bSuccess = true;
    }
    // 2. 소스와 타겟 슬롯이 같은 아이템이다 -> 병합
    else if (TargetSlot.ItemData == SourceSlot.ItemData)
    {
        int32 AmountToAdd = FMath::Min(SourceSlot.GetCount(), TargetSlot.GetRemainingCount());

        if (AmountToAdd > 0)
        {
            UpdateSlotCount(InTargetIndex, AmountToAdd);
            UpdateSlotCount(InSourceIndex, -AmountToAdd);

            OutResult.bSuccess = true;
        }
        else
        {
            OutResult.bSuccess = false;
        }
    }
    // 3. 소스와 타겟 슬롯이 서로 다른 아이템이다 -> 스왑
    else
    {
        const UItemDataAsset* SourceItem = SourceSlot.ItemData;
        int32 SourceCount = SourceSlot.GetCount();

        const UItemDataAsset* TargetItem = TargetSlot.ItemData;
        int32 TargetCount = TargetSlot.GetCount();

        SetSlot(InSourceIndex, TargetItem, TargetCount);
        SetSlot(InTargetIndex, SourceItem, SourceCount);

        OutResult.bSuccess = true;
    }

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleDropCommand(int32 InSlotIndex, const FVector& InDropLocation, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    FInvenSlot& Slot = Slots[InSlotIndex];

    if (Slot.IsEmpty())
    {
        return OutResult.bSuccess;
    }

    UWorld* World = GetWorld();
    const UItemDataAsset* ItemData = Slot.ItemData;

    if (World && ItemData)
    {
        if (UPickupFactorySubsystem* Factory = World->GetSubsystem<UPickupFactorySubsystem>())
        {
            for (int32 _ = 0; _ < Slot.GetCount(); _++)
            {
                FVector SpawnLocation(FMath::RandPointInCircle(100.0f), 0);
                SpawnLocation += InDropLocation;

                FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
                Factory->SpawnPickupAsync(const_cast<UItemDataAsset*>(ItemData), SpawnTransform, FOnPickupSpawned());
            }
        }

        ClearSlot(InSlotIndex);

        OutResult.bSuccess = true;
    }

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleUseCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    FInvenSlot& Slot = Slots[InSlotIndex];

    if (Slot.IsEmpty())
    {
        return OutResult.bSuccess;
    }

    UseItem(InSlotIndex);

    OutResult.bSuccess = true;
    OutResult.RemainingCount = Slot.GetRemainingCount();

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleClearCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    ClearSlot(InSlotIndex);
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleMoneyCommand(int32 InMoneyDiff, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    AddMoney(InMoneyDiff);
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleEquipCommand(int32 InSlotIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    FInvenSlot& Slot = Slots[InSlotIndex];

    if (Slot.IsEmpty())
    {
        return OutResult.bSuccess;
    }

    EquipItem(InSlotIndex);

    OutResult.bSuccess = true;
    OutResult.RemainingCount = Slot.GetRemainingCount();

    return OutResult.bSuccess;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

int32 UInventoryComponent::FindSlotWithItem(const UItemDataAsset* InItemData, int32 InStartIndex)
{
    int32 Result = InventoryFail;

    for (int32 i = InStartIndex; i < InventorySize; i++)
    {
        if (Slots[i].ItemData == InItemData && !Slots[i].IsFull())
        {
            Result = i;
            break;
        }
    }

    return Result;
}

int32 UInventoryComponent::FindEmptySlot()
{
    int32 Result = InventoryFail;

    for (int32 i = 0; i < InventorySize; i++)
    {
        if (Slots[i].IsEmpty())
        {
            Result = i;
            break;
        }
    }

    return Result;
}
