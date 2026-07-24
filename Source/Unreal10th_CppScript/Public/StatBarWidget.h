// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatBarWidget.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;
class UStatComponent;

UENUM(BlueprintType)
enum class EMyStatType : uint8
{
    None        UMETA(DisplayName = "None"),
    Heatlh      UMETA(DisplayName = "Health"),
    Stamina     UMETA(DisplayName = "Stamina")
};

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UStatBarWidget : public UUserWidget
{
    GENERATED_BODY()

    //public:
        //UFUNCTION(BlueprintCallable)
        //void TestImageColorChange(FLinearColor InColor);

        //UFUNCTION(BlueprintCallable)
        //void TestTextChange(FText InString);


protected:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    UFUNCTION()
    void HandleOnStatChange(float CurrentStat, float MaxStat);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMyStatType StatType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor ProgressBarColor;

protected:

    //// 변수이름 똑같게
    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    //TObjectPtr<UImage> TestImage;

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    //TObjectPtr<UTextBlock> TestTextBlock;

    //UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
    //TObjectPtr<UWidgetAnimation> TestAnimation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentStatTextBlock = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxStatTextBlock = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UProgressBar> StatProgressBar = nullptr;

private:
    UPROPERTY()
    TWeakObjectPtr<UStatComponent> StatComp = nullptr;

    float MaxStat = 0.0f;
    float CurrentStat = 0.0f;
    float ProgressBarValue = 0.0f;
};
