// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingActorByCurve.h"

void AFloatingActorByCurve::FloatingByCurveFloat(float DeltaTime, bool bLoop)
{
    ElapsedTime += DeltaTime;

    if (bLoop)
    {
        float MinTime = 0.0f;
        float MaxTime = 0.0f;

        CurveFloat->GetTimeRange(MinTime, MaxTime);

        if (ElapsedTime > MaxTime - MinTime)
        {
            ElapsedTime -= MaxTime - MinTime;
        }
    }

    MainMesh->SetRelativeLocation(FVector(0, 0, CurveFloat->GetFloatValue(ElapsedTime) * Distance));
}
