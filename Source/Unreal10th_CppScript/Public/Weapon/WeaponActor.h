// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class UCapsuleComponent;
class UWeaponDataAsset;
class UNiagaraComponent;

DECLARE_DELEGATE_OneParam(FOnWeaponDrop, UWeaponDataAsset*);

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AWeaponActor : public AActor
{
    GENERATED_BODY()

public:
    AWeaponActor();

    UFUNCTION(BlueprintCallable)
    void InitializeWeapon(UWeaponDataAsset* InData);

    UFUNCTION(BlueprintCallable)
    void EquipToTarget(AActor* InTarget);

    UFUNCTION(BlueprintCallable)
    void DropWeapon();

    UFUNCTION(BlueprintCallable)
    bool CanUse() const { return CurrentUseCount > 0; }

    UFUNCTION(BlueprintCallable)
    void Use();

    UFUNCTION(BlueprintCallable)
    void ResetUseCount();

    UFUNCTION(BlueprintCallable)
    FVector GetWeaponImpactLocation() const;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHitAreaBeginOverlap(
        UPrimitiveComponent* InOverlappedComponent,
        AActor* InOtherActor,
        UPrimitiveComponent* InOtherComp,
        int32 InOtherBodyIndex,
        bool bFromSweep,
        const FHitResult& InSweepResult
    );

    UFUNCTION(BlueprintCallable)
    void AttackEnable(bool bEnable);

    UFUNCTION(BlueprintCallable)
    void OnEquipped(AActor* InOwner);

public:
    FOnWeaponDrop OnWeaponDrop;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadonly)
    TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadonly)
    TObjectPtr<UCapsuleComponent> HitArea = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UNiagaraComponent> TrailVfx = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CurrentUseCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DropLiftSpan = 10.0f;

    // 드랍 직후 플레이어와의 물리 상호작용이 안되는 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PhysicsDelay = 2.0f;

private:
    TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr; // 무기를 장비하고 있는 캐릭터
    FTimerHandle PhysicsDelayTimerHandle;

};
