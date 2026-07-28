// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AttackEnable.generated.h"

//class AActionCharacter;
class IWeaponUserInterface;

/**
 *
 */
UCLASS()
class UNREAL10TH_CPPSCRIPT_API UAnimNotifyState_AttackEnable : public UAnimNotifyState
{
    GENERATED_BODY()

protected:
    virtual void NotifyBegin(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float TotalDuration,
        const FAnimNotifyEventReference& EventReference
    ) override;
    virtual void NotifyEnd(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;

private:
    //TWeakObjectPtr<AActionCharacter> OwnerCharacter = nullptr;
    IWeaponUserInterface* OwnerCharacter = nullptr;
};
