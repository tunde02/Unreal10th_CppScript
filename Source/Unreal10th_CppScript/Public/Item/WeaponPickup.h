// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AWeaponActor;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AWeaponPickup : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AWeaponPickup();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USphereComponent> SphereCollision = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<AWeaponActor> WeaponActorClass;
};
