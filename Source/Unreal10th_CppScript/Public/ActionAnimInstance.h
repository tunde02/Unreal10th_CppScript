// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ActionAnimInstance.generated.h"

class UPawnMovementComponent;

/**
 *
 */
UCLASS()
class UNREAL10TH_CPPSCRIPT_API UActionAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float MoveSpeed = 0.0f;

private:
    TWeakObjectPtr<UPawnMovementComponent> OwnerMovementComponent = nullptr;
};
