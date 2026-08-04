// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/DamagePopupWidget.h"
#include "Components/TextBlock.h"

void UDamagePopupWidget::SetDamageText(float InDamage)
{
    DamageText->SetText(FText::AsNumber(static_cast<int32>(InDamage)));
}

void UDamagePopupWidget::PlayPopupAnimation()
{
    PlayAnimation(PopupAnimation);
}

void UDamagePopupWidget::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
    Super::OnAnimationFinished_Implementation(Animation);

    if (Animation == PopupAnimation)
    {
        // 위젯 애니메이션이 종료됐음을 알림
        OnPopupAnimationFinished.ExecuteIfBound();
    }
}
