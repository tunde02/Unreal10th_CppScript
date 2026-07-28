// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

class UCapsuleComponent;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AWeaponActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AWeaponActor();

    UFUNCTION(BlueprintCallable)
    void OnEquipped(AActor* InOwner);

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

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadonly)
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadonly)
    TObjectPtr<UCapsuleComponent> HitArea = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName AttachSocketName = TEXT("hand_rSocket");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float WeaponDamage = 20.0f;

private:
    // 무기를 장비하고 있는 캐릭터
    TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr;
};
