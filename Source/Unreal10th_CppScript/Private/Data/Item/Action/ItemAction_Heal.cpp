// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/Action/ItemAction_Heal.h"
#include "Component/StatComponent.h"
#include "Interface/StatInterface.h"

void UItemAction_Heal::ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    UE_LOG(LogTemp, Log, TEXT("%s가 체력을 %.1f 회복합니다."), *InTarget->GetName(), HealAmount);
    if (IStatInterface* StatUser = Cast<IStatInterface>(InTarget))
    {
        if (UStatComponent* StatComp = StatUser->GetStatComponent())
        {
            IHealthInterface::Execute_HealHealth(StatComp, HealAmount);
        }
    }
}
