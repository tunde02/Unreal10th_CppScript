// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourceBarWidget.generated.h"

class UProgressBar;
class UTextBlock;

/**
 *
 */
UCLASS()
class UNREAL10TH_CPPSCRIPT_API UResourceBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void UpdateRecourceBar(float InCurrent, float InMax);

protected:
    virtual void NativePreConstruct() override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UProgressBar> Bar = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentText = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxText = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor FillColor = FLinearColor(0.0f, 1.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor BackgroundColor = FLinearColor(0.0f, 1.0f, 0.0f, 0.2f);
};
