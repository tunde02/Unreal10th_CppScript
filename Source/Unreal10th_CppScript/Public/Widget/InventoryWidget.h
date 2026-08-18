// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UMoneyPanelWidget;
struct FInvenSlot;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

private:
    void InitializeInventoryWidget();

    UFUNCTION()
    void ToggleInventoryWidget();

    UFUNCTION()
    void UpdateInventorySlots(const TArray<FInvenSlot>& InSlots);

    UFUNCTION()
    void UpdateMoneyPanel(int32 InMoney);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UMoneyPanelWidget> MoneyPanel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> SlotGridPanel;

private:
    bool bOpen = false;
};
