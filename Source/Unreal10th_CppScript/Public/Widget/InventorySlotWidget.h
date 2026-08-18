// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UInventoryComponent;
struct FInvenSlot;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    void InitializeInventorySlot(UInventoryComponent* InInventoryComponent, int32 InSlotIndex);
    void UpdateInventorySlot(const FInvenSlot& InSlot);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> IconImage = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> DisplayNameText = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CountText = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxStackText = nullptr;

private:
    TWeakObjectPtr<UInventoryComponent> InventoryComponent = nullptr;
    int32 SlotIndex = 0;

};
