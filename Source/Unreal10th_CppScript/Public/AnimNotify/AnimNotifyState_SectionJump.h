// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_SectionJump.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UAnimNotifyState_SectionJump : public UAnimNotifyState
{
    GENERATED_BODY()

public:
    inline FName GetNextSectionName() const { return NextSectionName; }

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


protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName NextSectionName;

};
