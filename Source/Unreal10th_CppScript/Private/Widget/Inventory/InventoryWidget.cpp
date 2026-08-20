// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventoryWidget.h"
#include "Widget/Inventory/InventorySlotWidget.h"
#include "Widget/Inventory/MoneyPanelWidget.h"
#include "Widget/Inventory/ItemDetailPanelWidget.h"
#include "Interface/InventoryUserInterface.h"
#include "Component/InventoryComponent.h"
#include "Player/ActionPlayerController.h"

#include "Components/Button.h"
#include "Components/UniformGridPanel.h"
#include "Components/CanvasPanelSlot.h"

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

                SlotWidget->OnSlotEnter.AddWeakLambda(
                    this,
                    [this](int InIndex) {
                        if (TargetInventory.IsValid())
                        {
                            ItemDetailPanel->Open(TargetInventory->GetSlot(InIndex)->ItemData);
                        }
                    }
                );
                SlotWidget->OnSlotLeave.AddWeakLambda(
                    this,
                    [this]() {
                        ItemDetailPanel->Close();
                    }
                );

                SlotWidgets.Add(SlotWidget);
            }
        }
    }

    RefreshInventoryWidget();

    CloseInventoryWidget();
}

void UInventoryWidget::ClearInventoryWidget()
{
    if (TargetInventory.IsValid())
    {
        TargetInventory->OnSlotChanged.Unbind();
        TargetInventory->OnMoneyChanged.RemoveAll(this);
        TargetInventory = nullptr;
    }

    SlotWidgets.Empty();
    SlotSize = 0;
}

void UInventoryWidget::OpenInventoryWidget()
{
    SetVisibility(ESlateVisibility::Visible);

    if (AActionPlayerController* PC = Cast<AActionPlayerController>(GetOwningPlayer()))
    {
        PC->OnInventoryOpenClose(true, this);
    }
}

void UInventoryWidget::CloseInventoryWidget()
{
    SetVisibility(ESlateVisibility::Collapsed);

    if (AActionPlayerController* PC = Cast<AActionPlayerController>(GetOwningPlayer()))
    {
        PC->OnInventoryOpenClose(false, this);
    }
}

void UInventoryWidget::ToggleInventoryWidget()
{
    if (IsInventoryOpen())
    {
        CloseInventoryWidget();
    }
    else
    {
        OpenInventoryWidget();
    }
}

void UInventoryWidget::TestRefresh()
{
    RefreshInventoryWidget();
}

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetIsFocusable(true);

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

    CloseInventoryWidget();
}

FReply UInventoryWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::I)
    {
        CloseInventoryWidget();

        // 이 입력에 대한 처리가 끝났다고 알림
        return FReply::Handled();
    }

    // 내가 처리하지 않은 입력은 부모에서 처리
    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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
