// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/Action/ItemAction_StaminaRecovery.h"
#include "Component/StatComponent.h"
#include "Interface/StatInterface.h"

void UItemAction_StaminaRecovery::ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    UE_LOG(LogTemp, Log, TEXT("%s가 스태미나를 %.1f 회복합니다."), *InTarget->GetName(), StaminaAmount);
    if (IStatInterface* StatUser = Cast<IStatInterface>(InTarget))
    {
        if (UStatComponent* StatComp = StatUser->GetStatComponent())
        {
            IStaminaInterface::Execute_RecoveryStamina(StatComp, StaminaAmount);
        }
    }
}
