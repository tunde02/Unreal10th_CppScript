// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FloatingActor.h"

#include "Curves/CurveFloat.h"

#include "FloatingActorByCurve.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AFloatingActorByCurve : public AFloatingActor
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void FloatingByCurveFloat(float DeltaTime, bool bLoop);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UCurveFloat> CurveFloat = nullptr;

    float ElapsedTime = 0.0f;
};
