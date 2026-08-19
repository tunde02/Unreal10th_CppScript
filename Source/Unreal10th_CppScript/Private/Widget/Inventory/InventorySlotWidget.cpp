// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventorySlotWidget.h"
#include "Component/InventoryComponent.h"

#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

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

void UInventorySlotWidget::UpdateInventorySlot(const FInvenSlot& InSlot)
{
    if (!InSlot.IsEmpty())
    {
        CountText->SetText(FText::AsNumber(InSlot.GetCount()));
        MaxStackText->SetText(FText::AsNumber(InSlot.ItemData->MaxStackCount));
    }
    else
    {
        CountText->SetText(FText::AsNumber(0));
        MaxStackText->SetText(FText::AsNumber(0));
    }
}
