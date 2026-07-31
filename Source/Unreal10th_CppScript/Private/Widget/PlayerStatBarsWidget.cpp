// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PlayerStatBarsWidget.h"
#include "Widget/ResourceBarWidget.h"
#include "Interface/StatInterface.h"
#include "Component/StatComponent.h"
#include "Interface/HealthInterface.h"
#include "Interface/StaminaInterface.h"

void UPlayerStatBarsWidget::NativeConstruct()
{
    Super::NativeConstruct();

    InitializePlayerStatBars();
}

void UPlayerStatBarsWidget::InitializePlayerStatBars()
{
    if (IStatInterface* Player = Cast<IStatInterface>(GetOwningPlayerPawn()))
    {
        if (UStatComponent* StatComp = Player->GetStatComponent())
        {
            StatComp->OnHealthChange.AddDynamic(HealthBar, &UResourceBarWidget::UpdateRecourceBar);
            StatComp->OnStaminaChange.AddDynamic(StaminaBar, &UResourceBarWidget::UpdateRecourceBar);

            HealthBar->UpdateRecourceBar(
                IHealthInterface::Execute_GetCurrentHealth(StatComp),
                IHealthInterface::Execute_GetMaxHealth(StatComp)
            );
            StaminaBar->UpdateRecourceBar(
                IStaminaInterface::Execute_GetCurrentStamina(StatComp),
                IStaminaInterface::Execute_GetMaxStamina(StatComp)
            );
        }
    }
}
