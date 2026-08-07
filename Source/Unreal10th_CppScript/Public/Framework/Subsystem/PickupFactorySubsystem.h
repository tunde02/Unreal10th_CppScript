// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PickupFactorySubsystem.generated.h"

class UItemDataAsset;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UPickupFactorySubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    void SpawnPickup(UItemDataAsset* InItemDataAsset, const FTransform& InTransform);

private:
    void InternalSpawnPickup(UItemDataAsset* InItemDataAsset, const FTransform& InTransform);
    void HandlePickupItemBounce(AActor* InActor) const;

};
