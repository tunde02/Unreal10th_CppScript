// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/ObjectPoolDataAsset.h"
#include "ObjectPoolSubsystem.generated.h"

class UNiagaraSystem;
class UObjectPoolDataAsset;

using FOrderNode = TDoubleLinkedList<TObjectPtr<AActor>>::TDoubleLinkedListNode;


USTRUCT()
struct FObjectPool
{
    GENERATED_BODY()

    FObjectPool()
        : ActiveOrderList(MakeShared<TDoubleLinkedList<TObjectPtr<AActor>>>()),
        ActiveNodeMap(MakeShared<TMap<TObjectPtr<AActor>, FOrderNode*>>())
    {
    }

    // 사용 대기 중인 액터들
    UPROPERTY(Transient)
    TArray<TObjectPtr<AActor>> ReadyActors;

    // 실제 사용 중인 액터들
    UPROPERTY(Transient)
    TSet<TObjectPtr<AActor>> ActiveActors;

    // 사용 순서를 기록할 더블 링크드 리스트(Head가 가장 오래됨, Tail이 가장 새것) (주의:GC가 추적은 못함)
    TSharedPtr<TDoubleLinkedList<TObjectPtr<AActor>>> ActiveOrderList;

    // 액터 포인터를 키값으로 하고, ActiveOrderList의 노드 주소를 Value로 하는 맵
    TSharedPtr<TMap<TObjectPtr<AActor>, FOrderNode*>> ActiveNodeMap;

    UPROPERTY(Transient)
    int32 InitialSize = 0;

    UPROPERTY(Transient)
    int32 MaxSize = 0;

    UPROPERTY(Transient)
    EObjectPoolPolicy MaxPolicy = EObjectPoolPolicy::Grow;

    int32 Size() const { return ReadyActors.Num() + ActiveActors.Num(); }
    bool IsFull() const { return Size() >= MaxSize; }

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
    AActor* CreateNewObject(TSubclassOf<AActor> InClassType, const FTransform& InTransform);
    AActor* GetReadyActor(FObjectPool* InPool);

protected:
    UPROPERTY()
    TMap<TSubclassOf<AActor>, FObjectPool> ObjectPools;

};
