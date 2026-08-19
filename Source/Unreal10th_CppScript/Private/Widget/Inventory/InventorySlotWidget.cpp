// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventorySlotWidget.h"
#include "Component/InventoryComponent.h"

#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // 중요: 드래그 감지 예약을 걸어둡니다. 
        // 유저가 움직이면 DragDetected로 가고, 안 움직이고 떼면 MouseButtonUp으로 갑니다.
        return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }

    return FReply::Unhandled();
}

FReply UInventorySlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply Reply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

    if (!TargetInventory.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : InventoryComponent nullptr"));
        return Reply;
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FInventoryCommandResult Result;
        TargetInventory->ExecuteCommand(
            FInventoryCommand::MakeUseCommand(Index),
            Result
        );
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        FInventoryCommandResult Result;
        TargetInventory->ExecuteCommand(
            FInventoryCommand::MakeDropCommand(Index, GetOwningPlayerPawn()->GetActorLocation()),
            Result
        );
    }

    return Reply;
}

void UInventorySlotWidget::InitializeSlot(UInventoryComponent* InInventoryComponent, int32 InSlotIndex)
{
    if (!InInventoryComponent)
    {
        return;
    }

    TargetInventory = InInventoryComponent;
    Index = InSlotIndex;
    Slot = TargetInventory->GetSlot(Index);

    RefreshSlot();
}

void UInventorySlotWidget::RefreshSlot() const
{
    if (!Slot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Slot %d]가 유효하지 않습니다."), Index);
        return;
    }

    if (Slot->IsEmpty())
    {
        IconImage->SetBrushFromTexture(nullptr);
        IconImage->SetBrushTintColor(FLinearColor::Transparent);
        CountBox->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        IconImage->SetBrushFromTexture(Slot->ItemData->Icon.Get());
        IconImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        CountText->SetText(FText::AsNumber(Slot->GetCount()));
        MaxStackText->SetText(FText::AsNumber(Slot->ItemData->MaxStackCount));
        CountBox->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    TargetInventory->OnSlotHovered.ExecuteIfBound(Slot);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    TargetInventory->OnSlotHovered.ExecuteIfBound(nullptr);

    Super::NativeOnMouseLeave(InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (!TargetInventory.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : InventoryComponent nullptr"));
        return;
    }

    if (!Slot || Slot->IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : Drag - 빈 슬롯 드래그 시도"));
        return;
    }

    FInventoryCommandResult Result;
    TargetInventory->ExecuteCommand(
        FInventoryCommand::MakeMoveCommand(Index, TargetInventory->GetSize()),
        Result
    );
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if (!TargetInventory.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : InventoryComponent nullptr"));
        return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
    }




    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{

    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}
