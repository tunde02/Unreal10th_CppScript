// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventorySlotWidget.h"
#include "Component/InventoryComponent.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    if (!InventoryComponent.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : InventoryComponent nullptr"));
        return Reply;
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        FInventoryCommandResult Result;
        InventoryComponent->ExecuteCommand(
            FInventoryCommand::MakeUseCommand(SlotIndex),
            Result
        );
    }
    else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        FInventoryCommandResult Result;

        InventoryComponent->ExecuteCommand(
            FInventoryCommand::MakeDropCommand(SlotIndex, GetOwningPlayerPawn()->GetActorLocation()),
            Result
        );
    }

    return Reply;
}

void UInventorySlotWidget::InitializeInventorySlot(UInventoryComponent* InInventoryComponent, int32 InSlotIndex)
{
    if (InInventoryComponent)
    {
        InventoryComponent = InInventoryComponent;
    }

    SlotIndex = InSlotIndex;

    DisplayNameText->SetText(FText::FromString(TEXT("빈 슬롯")));
    CountText->SetText(FText::AsNumber(0));
    MaxStackText->SetText(FText::AsNumber(0));
}

void UInventorySlotWidget::UpdateInventorySlot(const FInvenSlot& InSlot)
{
    if (!InSlot.IsEmpty())
    {
        DisplayNameText->SetText(InSlot.ItemData->DisplayName);
        CountText->SetText(FText::AsNumber(InSlot.GetCount()));
        MaxStackText->SetText(FText::AsNumber(InSlot.ItemData->MaxStackCount));
    }
    else
    {
        DisplayNameText->SetText(FText::FromString(TEXT("빈 슬롯")));
        CountText->SetText(FText::AsNumber(0));
        MaxStackText->SetText(FText::AsNumber(0));
    }
}
