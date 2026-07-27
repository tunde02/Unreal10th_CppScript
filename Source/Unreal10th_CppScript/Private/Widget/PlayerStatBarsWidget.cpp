// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PlayerStatBarsWidget.h"
#include "Widget/ResourceBarWidget.h"
#include "Interface/StatInterface.h"
#include "Interface/HealthInterface.h"
#include "Interface/StaminaInterface.h"
#include "Component/StatComponent.h"

void UPlayerStatBarsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializePlayerStatBars();
}

void UPlayerStatBarsWidget::InitializePlayerStatBars()
{
    if (IStatInterface* Player = Cast<IStatInterface>(GetOwningPlayerPawn()))
    {
        if (UStatComponent* Stat = Player->GetStatComponent())
        {
            Stat->OnHealthChange.AddDynamic(HealthBar, &UResourceBarWidget::UpdateRecourceBar);
            Stat->OnStaminaChange.AddDynamic(StaminaBar, &UResourceBarWidget::UpdateRecourceBar);

            HealthBar->UpdateRecourceBar(
                IHealthInterface::Execute_GetCurrentHealth(Stat),
                IHealthInterface::Execute_GetMaxHealth(Stat)
            );
            StaminaBar->UpdateRecourceBar(
                IStaminaInterface::Execute_GetCurrentStamina(Stat),
                IStaminaInterface::Execute_GetMaxStamina(Stat)
            );
        }
    }
}
