// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_SectionJump.h"
#include "Interface/WeaponUserInterface.h"
#include "Component/WeaponComponent.h"

void UAnimNotifyState_SectionJump::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (IWeaponUserInterface* WeaponOwner = Cast<IWeaponUserInterface>(MeshComp->GetOwner()))
    {
        if (UWeaponComponent* WeaponComp = WeaponOwner->GetWeaponComponent())
        {
            WeaponComp->SetSectionJumpNotify(this);
        }
    }
}

void UAnimNotifyState_SectionJump::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (IWeaponUserInterface* WeaponOwner = Cast<IWeaponUserInterface>(MeshComp->GetOwner()))
    {
        if (UWeaponComponent* WeaponComp = WeaponOwner->GetWeaponComponent())
        {
            WeaponComp->SetSectionJumpNotify(nullptr);
        }
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
