// Fill out your copyright notice in the Description page of Project Settings.


#include "Deprecated/StatBarWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Interface/StatInterface.h"
#include "Component/StatComponent.h"

//void UStatBarWidget::TestImageColorChange(FLinearColor InColor)
//{
//    TestImage->SetColorAndOpacity(InColor);
//}
//
//void UStatBarWidget::TestTextChange(FText InString)
//{
//    TestTextBlock->SetText(InString);
//}

void UStatBarWidget::NativeConstruct()
{
    Super::NativeConstruct();

    StatComp = Cast<IStatInterface>(GetOwningPlayerPawn())->GetStatComponent();

    switch (StatType)
    {
        case EMyStatType::Heatlh:
            StatComp->OnHealthChange.AddDynamic(this, &UStatBarWidget::HandleOnStatChange);
            break;
        case EMyStatType::Stamina:
            StatComp->OnStaminaChange.AddDynamic(this, &UStatBarWidget::HandleOnStatChange);
            break;
        case EMyStatType::None:
        default:
            break;
    }

    StatProgressBar->SetFillColorAndOpacity(ProgressBarColor);
}

void UStatBarWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (FMath::Abs(CurrentStat - ProgressBarValue) > 0.01f)
    {
        ProgressBarValue = FMath::Lerp(ProgressBarValue, CurrentStat, 0.05f);
        StatProgressBar->SetPercent(ProgressBarValue / MaxStat);
    }
}

void UStatBarWidget::HandleOnStatChange(float Current, float Max)
{
    ProgressBarValue = CurrentStat;
    CurrentStat = Current;
    MaxStat = Max;

    FString CurrentStatString = FString::Printf(TEXT("%.0f"), Current);
    FString MaxStatString = FString::Printf(TEXT("%.0f"), Max);

    CurrentStatTextBlock->SetText(FText::FromString(CurrentStatString));
    MaxStatTextBlock->SetText(FText::FromString(MaxStatString));
}
