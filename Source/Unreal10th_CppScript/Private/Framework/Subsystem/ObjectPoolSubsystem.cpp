// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Interface/PoolableInterface.h"
#include "Data/ObjectPoolDataAsset.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Containers/Map.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 프로젝트 세팅에서 데이터 읽어오기
    const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();

    if (!Settings)
    {
        return;
    }

    for (const TSoftObjectPtr<UObjectPoolDataAsset>& DataAsset : Settings->PoolableDataAssets)
    {
        if (!DataAsset.IsNull())
        {
            TObjectPtr<UObjectPoolDataAsset> LoadedDataAsset = DataAsset.LoadSynchronous();
            FObjectPool& Pool = ObjectPools.FindOrAdd(LoadedDataAsset->ActorClass.LoadSynchronous());
            Pool.InitialSize = LoadedDataAsset->InitialSize;
            Pool.MaxSize = LoadedDataAsset->MaxSize;
            Pool.MaxPolicy = LoadedDataAsset->MaxPolicy;

            UE_LOG(LogTemp, Log, TEXT("%s 풀 추가"), *LoadedDataAsset->GetName());
        }
    }
}

void UObjectPoolSubsystem::Deinitialize()
{
    ClearAllPools();

    Super::Deinitialize();
}

bool UObjectPoolSubsystem::RegisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset, bool bWarmup)
{
    if (!InDataAsset || InDataAsset->ActorClass.IsNull())
    {
        return false;
    }

    TSubclassOf<AActor> LoadedActorClass = InDataAsset->ActorClass.LoadSynchronous();

    ClearPool(LoadedActorClass);

    FObjectPool& Pool = ObjectPools.Add(LoadedActorClass);
    Pool.InitialSize = InDataAsset->InitialSize;

    if (bWarmup)
    {
        Warmup(LoadedActorClass);
    }

    return true;
}

bool UObjectPoolSubsystem::UnregisterPoolDataAsset(const UObjectPoolDataAsset* InDataAsset)
{
    if (!InDataAsset || InDataAsset->ActorClass.IsNull())
    {
        return false;
    }

    TSubclassOf<AActor> LoadedActorClass = InDataAsset->ActorClass.LoadSynchronous();

    ClearPool(LoadedActorClass);

    return true;
}

void UObjectPoolSubsystem::Warmup(TSubclassOf<AActor> InClass)
{
    FObjectPool* Pool = ObjectPools.Find(InClass);

    if (!Pool)
    {
        return;
    }

    FTransform InitialTransform(FVector::DownVector * 10000.0f);
    TArray<TWeakObjectPtr<AActor>> SpawnedArray;
    SpawnedArray.Reserve(Pool->InitialSize);

    for (int i = 0; i < Pool->InitialSize; i++)
    {
        SpawnedArray.Add(Spawn(InClass, InitialTransform));
    }

    for (TWeakObjectPtr<AActor> Spawned : SpawnedArray)
    {
        ReturnPool(Spawned.Get());
    }
}

void UObjectPoolSubsystem::WarmupAll()
{
    for (auto& [Key, _] : ObjectPools)
    {
        Warmup(Key);
    }
}

void UObjectPoolSubsystem::ClearPool(TSubclassOf<AActor> InClass)
{
    FObjectPool* Pool = ObjectPools.Find(InClass);

    if (!Pool)
    {
        return;
    }

    for (AActor* Actor : Pool->ReadyActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    Pool->ReadyActors.Empty();

    //for (AActor* Actor : Pool->ActiveActors)
    for (auto& [_, Actor] : Pool->ActiveActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    Pool->ActiveActors.Empty();

    ObjectPools.Remove(InClass);
}

void UObjectPoolSubsystem::ClearAllPools()
{
    for (auto& [Key, Pool] : ObjectPools)
    {
        for (AActor* Actor : Pool.ReadyActors)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }
        Pool.ReadyActors.Empty();

        //for (AActor* Actor : Pool.ActiveActors)
        for (auto& [_, Actor] : Pool.ActiveActors)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }
        Pool.ActiveActors.Empty();
    }

    ObjectPools.Empty();
}

AActor* UObjectPoolSubsystem::Spawn(TSubclassOf<AActor> InClassType, const FTransform& InTransform)
{
    if (!InClassType)
    {
        return nullptr;
    }

    FObjectPool* Pool = ObjectPools.Find(InClassType);

    if (!Pool)
    {
        return nullptr;
    }

    AActor* Spawned = nullptr;

    if (Pool->ReadyActors.Num() > 0)
    {
        Spawned = Pool->ReadyActors.Pop();
        Spawned->SetActorTransform(InTransform);

        UE_LOG(LogTemp, Log, TEXT("Spawn(Reuse) : %s"), *Spawned->GetName());
    }
    else if (Pool->IsFull())
    {
        switch (Pool->MaxPolicy)
        {
            case EObjectPoolPolicy::Grow:
            {
                if (GetWorld())
                {
                    FActorSpawnParameters SpawnParam;
                    SpawnParam.Owner = nullptr;
                    SpawnParam.ObjectFlags = RF_Transient;

                    Spawned = GetWorld()->SpawnActor<AActor>(InClassType, InTransform, SpawnParam);
#if WITH_EDITOR
                    if (Spawned)
                    {
                        Spawned->SetFolderPath(FName("Pool"));

                        UE_LOG(LogTemp, Log, TEXT("Pool Grow : %s"), *Spawned->GetName());
                    }
#endif
                }

                break;
            }
            case EObjectPoolPolicy::ReuseOldest:
            {
                double Key = Pool->ActivatedTimeSecondsQueue[0];

                if (TObjectPtr<AActor>* Value = Pool->ActiveActors.Find(Key))
                {
                    ReturnPool(*Value);

                    Spawned = Pool->ReadyActors.Pop();
                    Spawned->SetActorTransform(InTransform);

                    UE_LOG(LogTemp, Log, TEXT("Pool ReuseOldest : %s"), *Spawned->GetName());
                }
                break;
            }
            case EObjectPoolPolicy::DoNotSpawn:
                UE_LOG(LogTemp, Log, TEXT("Pool DoNotSpawn : "));
            default:
                break;
        }
    }
    else
    {
        if (GetWorld())
        {
            FActorSpawnParameters SpawnParam;
            SpawnParam.Owner = nullptr;
            SpawnParam.ObjectFlags = RF_Transient;

            Spawned = GetWorld()->SpawnActor<AActor>(InClassType, InTransform, SpawnParam);
#if WITH_EDITOR
            if (Spawned)
            {
                Spawned->SetFolderPath(FName("Pool"));

                UE_LOG(LogTemp, Log, TEXT("Spawn(New) : %s"), *Spawned->GetName());
            }
#endif
        }
    }

    if (Spawned)
    {
        if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
        {
            IPoolableInterface::Execute_OnSpawn(Spawned);
        }
        else
        {
            Spawned->SetActorHiddenInGame(false);
            Spawned->SetActorTickEnabled(true);
            Spawned->SetActorEnableCollision(true);
        }

        double ActivatedTimeSeconds = FPlatformTime::Seconds();
        Pool->ActiveActors.Add(ActivatedTimeSeconds, Spawned);
        Pool->ActivatedTimeSecondsQueue.HeapPush(ActivatedTimeSeconds);
    }

    return Spawned;
}

void UObjectPoolSubsystem::ReturnPool(AActor* InActor)
{
    if (!InActor)
    {
        return;
    }

    TSubclassOf<AActor> ActorClassType = InActor->GetClass();
    FObjectPool* Pool = ObjectPools.Find(ActorClassType);

    //if (!Pool || !Pool->ActiveActors.Contains(InActor))
    if (!Pool || !Pool->ActiveActors.FindKey(InActor))
    {
        return;
    }

    if (InActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
    {
        IPoolableInterface::Execute_OnReturn(InActor);
    }
    else
    {
        InActor->SetActorHiddenInGame(true);
        InActor->SetActorTickEnabled(false);
        InActor->SetActorEnableCollision(false);
        InActor->DisableComponentsSimulatePhysics();
    }

    //Pool->ActiveActors.Remove(InActor);
    double Key = *(Pool->ActiveActors.FindKey(InActor));
    Pool->ActiveActors.Remove(Key);
    Pool->ActivatedTimeSecondsQueue.HeapPop(Key);

    Pool->ReadyActors.Add(InActor);

    UE_LOG(LogTemp, Log, TEXT("ReturnPool() : %s"), *InActor->GetName());
}
