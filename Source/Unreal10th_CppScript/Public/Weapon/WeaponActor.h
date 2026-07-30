// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class UCapsuleComponent;
class UWeaponDataAsset;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AWeaponActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AWeaponActor();

    UFUNCTION(BlueprintCallable)
    void InitializeWeapon(UWeaponDataAsset* InData);

    UFUNCTION(BlueprintCallable)
    void EquipToTarget(AActor* InTarget);

    UFUNCTION(BlueprintCallable)
    void DropWeapon();

protected:
    // Called when the game starts or when spawned
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

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadonly)
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadonly)
    TObjectPtr<UCapsuleComponent> HitArea = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UWeaponDataAsset> WeaponData = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DropLiftSpan = 10.0f;

    // 드랍 직후 플레이어와의 물리 상호작용이 안되는 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PhysicsDelay = 2.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CurrentUseCount = -1;

private:
    // 무기를 장비하고 있는 캐릭터
    TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
    FTimerHandle PhysicsDelayTimerHandle;
};
