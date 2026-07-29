// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PickupBase.h"
#include "PickupWeapon.generated.h"

class AWeaponActor;
class UWeaponDataAsset;
class UCurveFloat;
class UNiagaraSystem;

/**
 *
 */
UCLASS()
class UNREAL10TH_CPPSCRIPT_API APickupWeapon : public APickupBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void OnPickup(AActor* InTarget) override;

private:
    void FloatingByCurve(float DeltaTime);
    void AbsorbToTarget();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UCurveFloat> CurveFloat = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UNiagaraSystem> AbsorbEffectSystem = nullptr;

private:
    bool bPickuped = false;
    AActor* Target = nullptr;
    float ElapsedTime = 0.0f;
    float AbsorbSpeed = 2.0f;
};
