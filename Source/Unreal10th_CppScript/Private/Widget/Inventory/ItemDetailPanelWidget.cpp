// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/ItemDetailPanelWidget.h"
#include "Component/InventoryComponent.h"
#include "Interface/InventoryUserInterface.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UItemDetailPanelWidget::RefreshDetailPanel(const FInvenSlot* InSlot)
{
    if (InSlot && !InSlot->IsEmpty())
    {
        IconImage->SetBrushFromTexture(InSlot->ItemData->Icon.Get());
        IconImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        DisplayNameText->SetText(InSlot->ItemData->DisplayName);
        PriceText->SetText(FText::AsNumber(InSlot->ItemData->Price));
        DescriptionText->SetText(InSlot->ItemData->Description);

        SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UItemDetailPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (IInventoryUserInterface* InventoryUser = Cast<IInventoryUserInterface>(GetOwningPlayerPawn()))
    {
        if (UInventoryComponent* InventoryComponent = InventoryUser->GetInventoryComponent())
        {
            InventoryComponent->OnSlotHovered.BindUObject(this, &UItemDetailPanelWidget::RefreshDetailPanel);
        }
    }
}
