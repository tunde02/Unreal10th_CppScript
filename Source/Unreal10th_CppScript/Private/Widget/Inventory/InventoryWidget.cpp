// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventoryWidget.h"
#include "Widget/Inventory/InventorySlotWidget.h"
#include "Widget/Inventory/MoneyPanelWidget.h"
#include "Interface/InventoryUserInterface.h"
#include "Component/InventoryComponent.h"

#include "Components/Button.h"
#include "Components/UniformGridPanel.h"

void UInventoryWidget::InitializeInventoryWidget(UInventoryComponent* InInventoryComponent)
{
    ClearInventoryWidget();

    TargetInventory = InInventoryComponent;

    if (!TargetInventory.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetInventory가 유효하지 않습니다."));
        return;
    }

    TargetInventory->OnSlotChanged.BindUObject(this, &UInventoryWidget::RefreshSlotWidget);
    TargetInventory->OnMoneyChanged.AddUObject(this, &UInventoryWidget::RefreshMoneyPanel);
    TargetInventory->OnInventoryAction.BindUObject(this, &UInventoryWidget::ToggleInventoryWidget);

    if (SlotGridPanel)
    {
        int32 ChildCount = SlotGridPanel->GetChildrenCount();
        int32 InventorySize = TargetInventory->GetSize();
        SlotSize = FMath::Min(ChildCount, InventorySize);

        SlotWidgets.Empty(SlotSize);
        for (int i = 0; i < SlotSize; i++)
        {
            if (UInventorySlotWidget* SlotWidget = Cast<UInventorySlotWidget>(SlotGridPanel->GetChildAt(i)))
            {
                SlotWidget->InitializeSlot(TargetInventory.Get(), i);
                SlotWidgets.Add(SlotWidget);
            }
        }
    }

    RefreshInventoryWidget();
}

void UInventoryWidget::ClearInventoryWidget()
{
    if (TargetInventory.IsValid())
    {
        TargetInventory->OnSlotChanged.Unbind();
        TargetInventory->OnMoneyChanged.Clear();
        TargetInventory = nullptr;
    }

    SlotSize = 0;
}

void UInventoryWidget::OpenInventoryWidget()
{
    SetVisibility(ESlateVisibility::Visible);
}

void UInventoryWidget::CloseInventoryWidget()
{
    SetVisibility(ESlateVisibility::Collapsed);
}

void UInventoryWidget::TestRefresh()
{
    RefreshInventoryWidget();
}

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnClickedCloseButton);
    }

    if (IInventoryUserInterface* InventoryUser = Cast<IInventoryUserInterface>(GetOwningPlayerPawn()))
    {
        if (UInventoryComponent* InventoryComponent = InventoryUser->GetInventoryComponent())
        {
            InitializeInventoryWidget(InventoryComponent);
        }
    }
}

void UInventoryWidget::RefreshInventoryWidget() const
{
    if (!TargetInventory.IsValid())
    {
        return;
    }

    RefreshMoneyPanel(TargetInventory->GetMoney());

    for (const UInventorySlotWidget* SlotWidget : SlotWidgets)
    {
        if (SlotWidget)
        {
            SlotWidget->RefreshSlot();
        }
    }
}

void UInventoryWidget::RefreshSlotWidget(int32 InSlotIndex) const
{
    if (!IsValidIndex(InSlotIndex) || !SlotWidgets[InSlotIndex])
    {
        return;
    }

    SlotWidgets[InSlotIndex]->RefreshSlot();
}

void UInventoryWidget::RefreshMoneyPanel(int32 InCurrentMoney) const
{
    if (!MoneyPanel)
    {
        return;
    }

    MoneyPanel->SetMoney(InCurrentMoney);
}

void UInventoryWidget::OnClickedCloseButton()
{
    CloseInventoryWidget();
}

void UInventoryWidget::ToggleInventoryWidget()
{
    bOpen = !bOpen;
    if (bOpen)
    {
        OpenInventoryWidget();
    }
    else
    {
        CloseInventoryWidget();
    }
}
