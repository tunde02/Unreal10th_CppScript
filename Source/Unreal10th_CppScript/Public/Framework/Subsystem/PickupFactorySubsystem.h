// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "PickupFactorySubsystem.generated.h"

class APickupBase;
class UItemDataAsset;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnPickupSpawnedDynamic, APickupBase*, SpawnedPickup);
DECLARE_DELEGATE_OneParam(FOnPickupSpawned, APickupBase*);

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UPickupFactorySubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Factory|Pickup")
    APickupBase* SpawnPickup(UItemDataAsset* InItemDataAsset, const FTransform& InTransform);

    void SpawnPickupAsync(UItemDataAsset* InItemDataAsset, const FTransform& InTransform, FOnPickupSpawned OnSpawned);

    UFUNCTION(BlueprintCallable, Category = "Factory|Pickup", meta = (DisplayName = "Spawn Pickup Async"))
    void K2_SpawnPickupAsync(UItemDataAsset* InItemDataAsset, const FTransform& InTransform, FOnPickupSpawnedDynamic OnSpawned);

private:
    APickupBase* SpawnProcess(UItemDataAsset* InItemDataAsset, const FTransform& InTransform);
    void HandlePickupItemBounce(AActor* InActor) const;

    // 비동기 작업의 핸들 중 완료된 핸들을 정리하는 함수
    void CleanupCompletedHandles();

protected:
    // 진행중인 비동기 작업의 핸들 모음
    TArray<TSharedPtr<FStreamableHandle>> ActiveStreamableHandles;

};
