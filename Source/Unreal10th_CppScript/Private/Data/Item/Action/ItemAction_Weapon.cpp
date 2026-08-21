// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/Action/ItemAction_Weapon.h"
#include "Component/WeaponComponent.h"
#include "Interface/WeaponUserInterface.h"
#include "Data/Item/WeaponDataAsset.h"

void UItemAction_Weapon::ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    if (!WeaponData)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ItemAction_Weapon] WeaponData nullptr."));
        return;
    }

    if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(InTarget))
    {
        if (UWeaponComponent* WeaponComp = WeaponUser->GetWeaponComponent())
        {
            WeaponComp->EquipWeapon(WeaponData);
            //IWeaponUserInterface::Execute_EquipWeapon(WeaponUser, WeaponData);
            UE_LOG(LogTemp, Log, TEXT("%s가 %s를 장착했습니다."),
                   *InTarget->GetName(), *WeaponData->DisplayName.ToString());
        }
    }
}
