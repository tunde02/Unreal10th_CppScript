// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"
#include "Framework/Subsystem/PickupFactorySubsystem.h"

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
            if (OutResult.bSuccess)
            {
                UE_LOG(LogTemp, Log, TEXT("[%s] 가 성공적으로 인벤토리에 추가되었습니다."), *Command.ItemData->DisplayName.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("%d개의 아이템이 남았습니다."), OutResult.RemainingCount);
            }
            break;
        case EInventoryCommandType::Move:
            HandleMoveCommand(Command.ItemData, Command.Count, Command.SourceIndex, Command.TargetIndex, OutResult);
            if (OutResult.bSuccess)
            {
                UE_LOG(LogTemp, Log, TEXT("[%s] 가 성공적으로 %d번 슬롯에 옮겨졌습니다."), *Command.ItemData->DisplayName.ToString(), Command.TargetIndex);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("아이템 슬롯 옮기기 실패.."), OutResult.RemainingCount);
            }
            break;
        case EInventoryCommandType::Drop:
            HandleDropCommand(Command.ItemData, Command.Count, Command.SourceIndex, Command.DropTransform, OutResult);
            break;
        case EInventoryCommandType::Use:
            HandleUseCommand(Command.ItemData, Command.Count, Command.SourceIndex, OutResult);
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
    return &Slots[InventorySize];
}

void UInventoryComponent::AddMoney(int32 InIncome)
{
    Money += InIncome;
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
        UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.UseItem()] : 유효하지 않은 인덱스 (Index: %d)"), InSlotIndex);
        return;
    }

    FInvenSlot& SourceSlot = Slots[InSlotIndex];
    if (SourceSlot.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.UseItem()] : Index 슬롯에 아이템이 없음"));
        return;
    }

    FInventoryCommand UseCommand = FInventoryCommand::MakeUseCommand(SourceSlot.ItemData, SourceSlot.GetCount(), InSlotIndex);
    FInventoryCommandResult Result;
    ExecuteCommand(UseCommand, Result);
}

void UInventoryComponent::MoveItem(int32 InSourceIndex, int32 InTargetIndex)
{
    if (!IsValidIndex(InSourceIndex) || !IsValidIndex(InTargetIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.MoveItem()] : 유효하지 않은 인덱스 (SourceIndex: %d, TargetIndex: %d)"),
               InSourceIndex, InTargetIndex);
        return;
    }

    FInvenSlot& SourceSlot = Slots[InSourceIndex];
    if (SourceSlot.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.MoveItem()] : SourceIndex 슬롯에 아이템이 없음"));
        return;
    }

    FInvenSlot& TargetSlot = Slots[InTargetIndex];
    if (!TargetSlot.IsEmpty() && SourceSlot.ItemData != TargetSlot.ItemData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.MoveItem()] : TargetIndex 슬롯이 비어있지 않고 SourceIndex 슬롯의 아이템과 서로 다름"));
        return;
    }

    FInventoryCommand MoveCommand = FInventoryCommand::MakeMoveCommand(SourceSlot.ItemData, SourceSlot.GetCount(), InSourceIndex, InTargetIndex);
    FInventoryCommandResult Result;
    ExecuteCommand(MoveCommand, Result);
}

void UInventoryComponent::DropItem(int32 InIndex, FTransform InTransform)
{
    if (!IsValidIndex(InIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.DropItem()] : 유효하지 않은 인덱스 (Index: %d)"), InIndex);
        return;
    }

    FInvenSlot& Slot = Slots[InIndex];
    if (Slot.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventoryComponent.DropItem()] : SourceIndex 슬롯에 아이템이 없음"));
        return;
    }

    FInventoryCommand DropCommand = FInventoryCommand::MakeDropCommand(Slot.ItemData, Slot.GetCount(), InIndex, InTransform);
    FInventoryCommandResult Result;
    ExecuteCommand(DropCommand, Result);
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
    Slot.SetCount(NewCount);
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

    // 델리게이트 전담 함수
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

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleMoveCommand(const UItemDataAsset* InItemData, int32 InCount, int32 InSourceIndex, int32 InTargetIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(InTargetIndex))
    {
        return OutResult.bSuccess;
    }

    if (!IsValidIndex(InSourceIndex))
    {
        return OutResult.bSuccess;
    }

    FInvenSlot& Slot = Slots[InTargetIndex];
    Slot.ItemData = InItemData;
    Slot.SetCount(InCount);

    Slots[InSourceIndex].Clear();
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleDropCommand(const UItemDataAsset* InItemData, int32 InCount, int32 InSlotIndex, FTransform InTransform, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    FInvenSlot& Slot = Slots[InSlotIndex];

    UPickupFactorySubsystem* Factory = GetWorld()->GetSubsystem<UPickupFactorySubsystem>();
    Factory->SpawnPickup(const_cast<UItemDataAsset*>(Slot.ItemData.Get()), InTransform); // const_cast..??

    Slots[InSlotIndex].Clear();
    OutResult.bSuccess = true;

    return OutResult.bSuccess;
}

bool UInventoryComponent::HandleUseCommand(const UItemDataAsset* InItemData, int32 InCount, int32 InSlotIndex, FInventoryCommandResult& OutResult)
{
    OutResult.bSuccess = false;

    if (!IsValidIndex(InSlotIndex))
    {
        return OutResult.bSuccess;
    }

    UpdateSlotCount(InSlotIndex, -1);
    UE_LOG(LogTemp, Log, TEXT("[InventoryComponent.HandleUseCommand()] : %s를 사용했습니다."), *InItemData->DisplayName.ToString());

    OutResult.bSuccess = true;

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
