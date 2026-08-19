// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemDetailPanelWidget.generated.h"

class UImage;
class UTextBlock;
struct FInvenSlot;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UItemDetailPanelWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void RefreshDetailPanel(const FInvenSlot* InSlot);

protected:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> IconImage = nullptr;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> DisplayNameText = nullptr;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> PriceText = nullptr;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> DescriptionText = nullptr;

};
