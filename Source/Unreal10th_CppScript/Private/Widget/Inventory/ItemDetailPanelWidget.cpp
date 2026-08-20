// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/ItemDetailPanelWidget.h"
#include "Component/InventoryComponent.h"
#include "Interface/InventoryUserInterface.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UItemDetailPanelWidget::Open(const UItemDataAsset* InItemData)
{
    if (!InItemData)
    {
        return;
    }

    IconImage->SetBrushFromTexture(InItemData->Icon.Get());
    DisplayNameText->SetText(InItemData->DisplayName);
    PriceText->SetText(FText::AsNumber(InItemData->Price));
    DescriptionText->SetText(InItemData->Description);

    SetVisibility(ESlateVisibility::HitTestInvisible);

    bTickEnabled = true;
}

void UItemDetailPanelWidget::Close()
{
    SetVisibility(ESlateVisibility::Collapsed);
    bTickEnabled = false;
}

void UItemDetailPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    CanvasSlot = Cast<UCanvasPanelSlot>(Slot);

    Close();
}

void UItemDetailPanelWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!bTickEnabled)
    {
        return;
    }

    UpdateLocation();
}

void UItemDetailPanelWidget::UpdateLocation()
{
    if (!CanvasSlot.IsValid())
    {
        CanvasSlot = Cast<UCanvasPanelSlot>(Slot);
    }

    if (CanvasSlot.IsValid())
    {
        const FVector2D MouseAbsolute = UWidgetLayoutLibrary::GetMousePositionOnPlatform();
        if (UPanelWidget* ParentPanel = GetParent())
        {
            const FGeometry& ParentGeometry = ParentPanel->GetTickSpaceGeometry();
            const FVector2D LocalPos = ParentGeometry.AbsoluteToLocal(MouseAbsolute);
            CanvasSlot->SetPosition(LocalPos);
        }
    }
}
