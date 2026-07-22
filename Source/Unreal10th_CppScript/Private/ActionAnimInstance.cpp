// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

void UActionAnimInstance::NativeInitializeAnimation()
{
    if (APawn* OwnerPawn = TryGetPawnOwner())
    {
        OwnerMovementComponent = OwnerPawn->GetMovementComponent();
    }
}

void UActionAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (OwnerMovementComponent)
    {
        MoveSpeed = OwnerMovementComponent->Velocity.Size();
    }
    else
    {
        if (APawn* OwnerPawn = TryGetPawnOwner())
        {
            OwnerMovementComponent = OwnerPawn->GetMovementComponent();
        }
    }
}
