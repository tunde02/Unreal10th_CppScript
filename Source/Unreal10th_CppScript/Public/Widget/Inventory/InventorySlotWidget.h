// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

class UImage;
class UHorizontalBox;
class UTextBlock;
class UInventoryComponent;
struct FInvenSlot;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSlotEnter, int32);
DECLARE_MULTICAST_DELEGATE(FOnSlotLeave);

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    void InitializeSlot(UInventoryComponent* InInventoryComponent, int32 InSlotIndex);
    void RefreshSlot() const;

protected:
    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

public:
    FOnSlotEnter OnSlotEnter;
    FOnSlotLeave OnSlotLeave;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> IconImage = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UHorizontalBox> CountBox = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CountText = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxStackText = nullptr;

private:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<UInventoryComponent> TargetInventory = nullptr;

    static constexpr int32 InvalidIndex = -1;

    int32 Index = InvalidIndex;

    const FInvenSlot* Slot = nullptr;
};
