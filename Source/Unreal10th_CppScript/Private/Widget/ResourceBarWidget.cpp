// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ResourceBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UResourceBarWidget::UpdateRecourceBar(float InCurrent, float InMax)
{
    Bar->SetPercent(InCurrent / InMax);
    CurrentText->SetText(FText::AsNumber(FMath::FloorToInt(InCurrent)));
    MaxText->SetText(FText::AsNumber(FMath::FloorToInt(InMax)));
}

void UResourceBarWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    Bar->SetFillColorAndOpacity(FillColor);

    FProgressBarStyle Style = Bar->GetWidgetStyle();
    Style.BackgroundImage.TintColor = BackgroundColor;
    Bar->SetWidgetStyle(Style);
}

#if WITH_EDITOR
void UResourceBarWidget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName TargetPropertyName =
        (PropertyChangedEvent.Property != nullptr)
        ? PropertyChangedEvent.Property->GetFName()
        : NAME_None;

    // 가져온 프로퍼티 이름이 UResourceBarWidget의 FillColor와 같은지 확인
    if (TargetPropertyName == GET_MEMBER_NAME_CHECKED(UResourceBarWidget, FillColor))
    {
        BackgroundColor = FillColor;
        BackgroundColor.A = 0.2f;
    }
}
#endif