// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDetailPanelWidget.generated.h"

class UImage;
class UTextBlock;
class UItemDataAsset;
class UCanvasPanelSlot;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UItemDetailPanelWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Open(const UItemDataAsset* InItemData);
    void Close();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    void UpdateLocation();

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> IconImage = nullptr;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> DisplayNameText = nullptr;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> PriceText = nullptr;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> DescriptionText = nullptr;

private:
    bool bTickEnabled = false;
    TWeakObjectPtr<UCanvasPanelSlot> CanvasSlot;
    FVector2D ParentPosition;

};
