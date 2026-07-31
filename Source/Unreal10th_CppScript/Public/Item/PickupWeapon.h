// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PickupBase.h"
#include "PickupWeapon.generated.h"

class AWeaponActor;
class UWeaponDataAsset;
class UNiagaraSystem;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API APickupWeapon : public APickupBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

    virtual void OnPickup(AActor* InTarget) override;
    virtual void OnUpdatePickupEffect();
    virtual void OnFinishPickupEffect();

private:
    bool IsPickupEffectAssetReady() const;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data")
    TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    TObjectPtr<UCurveFloat> PickupAlpha;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    TObjectPtr<UCurveFloat> PickupHeight;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    TObjectPtr<UCurveFloat> PickupScale;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    float PickupEffectDuration = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    float PickupEffectHeight = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Pickup")
    TObjectPtr<UNiagaraSystem> PickupVfx = nullptr;

private:
    // 아이템을 줍는 연출용 타이머의 실행 간격
    const float TimerInterval = 0.02f;

    FTimerHandle PickupEffectTimerHandle;
    TWeakObjectPtr<AActor> TargetActor = nullptr;
    float PickupElapsedTime = 0.0f;
    FVector PickupStartLocation;

};
