// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamagePopupWidget.generated.h"

DECLARE_DELEGATE(FOnPopupAnimationFinished)

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UDamagePopupWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetDamageText(float InDamage);

    UFUNCTION(BlueprintCallable)
    void PlayPopupAnimation();

protected:
    virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;

public:
    FOnPopupAnimationFinished OnPopupAnimationFinished;

protected:
    UPROPERTY(VisibleDefaultsOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> DamageText;

    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> PopupAnimation;

};
