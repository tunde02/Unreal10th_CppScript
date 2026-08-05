// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

class UNiagaraSystem;
class UObjectPoolDataAsset;

USTRUCT()
struct FObjectPool
{
    GENERATED_BODY()

    // 사용 대기 중인 액터들
    UPROPERTY(Transient)
    TArray<TObjectPtr<AActor>> ReadyActors;

    // 실제 사용 중인 액터들
    UPROPERTY(Transient)
    TSet<TObjectPtr<AActor>> ActiveActors;

    UPROPERTY(Transient)
    int32 InitialSize = 0;

};

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UObjectPoolSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection);
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable)
    bool RegisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset, bool bWarmup = false);

    UFUNCTION(BlueprintCallable)
    bool UnregisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset);

    UFUNCTION(BlueprintCallable)
    void Warmup(TSubclassOf<AActor> InClass);

    UFUNCTION(BlueprintCallable)
    void WarmupAll();

    UFUNCTION(BlueprintCallable)
    void ClearPool(TSubclassOf<AActor> InClass);

    UFUNCTION(BlueprintCallable)
    void ClearAllPools();

    UFUNCTION(BlueprintCallable)
    AActor* Spawn(TSubclassOf<AActor> InClassType, const FTransform& InTransform);

    UFUNCTION(BlueprintCallable)
    void ReturnPool(AActor* InActor);

    template<typename T>
    T* Spawn(TSubclassOf<T> InClassType, const FTransform& InTransform)
    {
        return Cast<T>(Spawn(TSubclassOf<AActor>(InClassType), InTransform));
    }

protected:
    UPROPERTY()
    TMap<TSubclassOf<AActor>, FObjectPool> ObjectPools;

};
