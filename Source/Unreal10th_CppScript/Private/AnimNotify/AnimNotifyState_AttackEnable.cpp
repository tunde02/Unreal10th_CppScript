// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_AttackEnable.h"

#include "Player/ActionCharacter.h"
#include "Component/WeaponComponent.h"

void UAnimNotifyState_AttackEnable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (IWeaponUserInterface* WeaponOwner = Cast<IWeaponUserInterface>(MeshComp->GetOwner()))
    {
        if (UWeaponComponent* WeaponComp = WeaponOwner->GetWeaponComponent())
        {
            WeaponComp->OnWeaponAttackState(true);
        }
    }
}

void UAnimNotifyState_AttackEnable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (IWeaponUserInterface* WeaponOwner = Cast<IWeaponUserInterface>(MeshComp->GetOwner()))
    {
        if (UWeaponComponent* WeaponComp = WeaponOwner->GetWeaponComponent())
        {
            WeaponComp->OnWeaponAttackState(false);
        }
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
