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

public:
    APickupWeapon();

    virtual void InitializePickup(UItemDataAsset* InData) override;

protected:
    virtual void BeginPlay() override;
    //virtual void OnFinishPickupEffect() override;
    virtual UMeshComponent* GetMesh() const override;

private:
    //bool IsPickupEffectAssetReady() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Pickup")
    TObjectPtr<UNiagaraSystem> PickupVfx = nullptr;

private:
    TWeakObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

};
