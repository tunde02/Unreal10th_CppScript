// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_AttackEnable.h"
#include "Player/ActionCharacter.h"

void UAnimNotifyState_AttackEnable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    OwnerCharacter = Cast<IWeaponUserInterface>(MeshComp->GetOwner());
    if (OwnerCharacter)
    {
        OwnerCharacter->OnWeaponAttackState(true);
    }
}

void UAnimNotifyState_AttackEnable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (OwnerCharacter)
    {
        OwnerCharacter->OnWeaponAttackState(false);
        OwnerCharacter = nullptr;
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
