// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UButton;
class UUniformGridPanel;
class UMoneyPanelWidget;
class UInventoryComponent;
class UInventorySlotWidget;
class UItemDetailPanelWidget;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 인벤토리 컴포넌트와 인벤토리 위젯을 연결하는 초기화용 함수
    void InitializeInventoryWidget(UInventoryComponent* InInventoryComponent);

    // 인벤토리 위젯과 인벤토리 컴포넌트 사이의 연결을 제거하는 함수
    void ClearInventoryWidget();
    void OpenInventoryWidget();
    void CloseInventoryWidget();
    void ToggleInventoryWidget();

    bool IsInventoryOpen() const { return GetVisibility() == ESlateVisibility::Visible; }

#if WITH_EDITOR
    void TestRefresh();
#endif

protected:
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    void RefreshInventoryWidget() const;
    void RefreshSlotWidget(int32 InSlotIndex) const;
    void RefreshMoneyPanel(int32 InCurrentMoney) const;

private:
    UFUNCTION()
    void OnClickedCloseButton();

    inline bool IsValidIndex(int32 InIndex) const { return 0 <= InIndex && InIndex < SlotSize; }

protected:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> CloseButton;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UMoneyPanelWidget> MoneyPanel;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> SlotGridPanel;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UItemDetailPanelWidget> ItemDetailPanel;

private:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    TWeakObjectPtr<UInventoryComponent> TargetInventory = nullptr;

    UPROPERTY()
    TArray<TObjectPtr<UInventorySlotWidget>> SlotWidgets;

    int32 SlotSize = 0;

};
