// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotifyState_SectionJump.h"
#include "Player/ActionCharacter.h"

void UAnimNotifyState_SectionJump::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    AActionCharacter* OwnerCharacter = Cast<AActionCharacter>(MeshComp->GetOwner());
    if (OwnerCharacter)
    {
        OwnerCharacter->SetSectionJumpNotify(this);
    }
}

void UAnimNotifyState_SectionJump::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    AActionCharacter* OwnerCharacter = Cast<AActionCharacter>(MeshComp->GetOwner());
    if (OwnerCharacter)
    {
        OwnerCharacter->SetSectionJumpNotify(nullptr);
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
