// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/InventoryWidget.h"
#include "Widget/InventorySlotWidget.h"
#include "Widget/MoneyPanelWidget.h"
#include "Interface/InventoryUserInterface.h"
#include "Component/InventoryComponent.h"

#include "Components/UniformGridPanel.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializeInventoryWidget();
}

void UInventoryWidget::InitializeInventoryWidget()
{
    if (IInventoryUserInterface* Player = Cast<IInventoryUserInterface>(GetOwningPlayerPawn()))
    {
        UInventoryComponent* InventoryComponent = Player->GetInventoryComponent();

        // Bind to UInventoryComponent::FOnInventoryAction delegate
        InventoryComponent->OnInventoryAction.AddDynamic(this, &UInventoryWidget::ToggleInventoryWidget);

        // Bind to UInventoryComponent::FOnInventoryChange delegate
        InventoryComponent->OnInventoryChange.AddDynamic(this, &UInventoryWidget::UpdateInventorySlots);

        // Initialize Inventory Slots
        for (int i = 0; i < SlotGridPanel->GetChildrenCount(); i++)
        {
            UInventorySlotWidget* InventorySlot = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(i));
            InventorySlot->InitializeInventorySlot(InventoryComponent, i);
        }

        // Bind to UInventoryComponent::FOnMoneyChange delegate
        InventoryComponent->OnMoneyChange.AddDynamic(this, &UInventoryWidget::UpdateMoneyPanel);
    }
}

void UInventoryWidget::ToggleInventoryWidget()
{
    bOpen = !bOpen;
    ESlateVisibility CurrentVisibility = bOpen ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;
    SetVisibility(CurrentVisibility);
}

void UInventoryWidget::UpdateInventorySlots(const TArray<FInvenSlot>& InSlots)
{
    for (int i = 0; i < SlotGridPanel->GetChildrenCount(); i++)
    {
        UInventorySlotWidget* InventorySlot = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(i));
        InventorySlot->UpdateInventorySlot(InSlots[i]);
    }
}

void UInventoryWidget::UpdateMoneyPanel(int32 InMoney)
{
    MoneyPanel->UpdateMoneyText(InMoney);
}
