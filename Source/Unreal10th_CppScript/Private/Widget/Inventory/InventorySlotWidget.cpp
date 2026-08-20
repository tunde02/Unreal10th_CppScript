// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventorySlotWidget.h"
#include "Widget/Inventory/InventoryDragDropOperation.h"
#include "Widget/Inventory/TemporarySlotWidget.h"
#include "Component/InventoryComponent.h"

#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::InitializeSlot(UInventoryComponent* InInventoryComponent, int32 InSlotIndex)
{
    if (!InInventoryComponent)
    {
        return;
    }

    TargetInventory = InInventoryComponent;
    Index = InSlotIndex;

    RefreshSlot();
}

void UInventorySlotWidget::RefreshSlot() const
{
    if (!TargetInventory.IsValid())
    {
        return;
    }

    const FInvenSlot* TargetSlot = TargetInventory->GetSlot(Index);
    if (!TargetSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Slot %d]가 유효하지 않습니다."), Index);
        return;
    }

    if (TargetSlot->IsEmpty())
    {
        IconImage->SetBrushFromTexture(nullptr);
        IconImage->SetBrushTintColor(FLinearColor::Transparent);
        CountBox->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        IconImage->SetBrushFromTexture(TargetSlot->ItemData->Icon.Get());
        IconImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        CountText->SetText(FText::AsNumber(TargetSlot->GetCount()));
        MaxStackText->SetText(FText::AsNumber(TargetSlot->ItemData->MaxStackCount));
        CountBox->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    OnSlotEnter.Broadcast(Index);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    OnSlotLeave.Broadcast();

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
