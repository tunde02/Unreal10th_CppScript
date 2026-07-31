// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupEffect.h"
#include "Component/StatComponent.h"
#include "Interface/StatInterface.h"
#include "Interface/StaminaInterface.h"
#include "Interface/HealthInterface.h"

void APickupEffect::OnPickup(AActor* InTarget)
{
    if (IStatInterface* Stat = Cast<IStatInterface>(InTarget))
    {
        if (UStatComponent* StatComponent = Stat->GetStatComponent())
        {
            if (Stamina > 0.0f)
            {
                IStaminaInterface::Execute_RecoveryStamina(StatComponent, Stamina);
            }
            else if (Stamina < 0.0f)
            {
                IStaminaInterface::Execute_ConsumeStamina(StatComponent, -Stamina);
            }

            if (Health > 0.0f)
            {
                IHealthInterface::Execute_HealHealth(StatComponent, Health);
            }
            else if (Health < 0.0f)
            {
                IHealthInterface::Execute_DamageHealth(StatComponent, -Health);
            }
        }
    }
}
