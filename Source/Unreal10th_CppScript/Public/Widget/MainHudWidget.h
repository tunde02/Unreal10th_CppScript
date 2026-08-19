// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHudWidget.generated.h"

class UPlayerStatBarsWidget;
class UInventoryWidget;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UMainHudWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void TestInventoryRefresh() const;

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UPlayerStatBarsWidget> PlayerStatBars;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UInventoryWidget> Inventory;

};
