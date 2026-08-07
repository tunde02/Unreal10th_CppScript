// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Interface/PoolableInterface.h"
#include "Data/ObjectPoolDataAsset.h"

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

            //UE_LOG(LogTemp, Log, TEXT("%s 풀 추가"), *LoadedDataAsset->GetName());
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
    Pool.MaxSize = InDataAsset->MaxSize;
    Pool.MaxPolicy = InDataAsset->MaxPolicy;

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

    for (int _ = 0; _ < Pool->InitialSize; _++)
    {
        AActor* Spawned = CreateNewObject(InClass, InitialTransform);

        if (Spawned)
        {
            if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
            {
                IPoolableInterface::Execute_OnSpawn(Spawned);
            }
            else
            {
                Spawned->SetActorHiddenInGame(true);
                Spawned->SetActorTickEnabled(false);
                Spawned->SetActorEnableCollision(false);
            }

            Pool->ReadyActors.Add(Spawned);
        }
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

    for (AActor* Actor : Pool->ActiveActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    Pool->ActiveActors.Empty();
    Pool->ActiveOrderList->Empty();
    Pool->ActiveNodeMap->Empty();

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

        for (AActor* Actor : Pool.ActiveActors)
        {
            if (IsValid(Actor))
            {
                Actor->Destroy();
            }
        }
        Pool.ActiveActors.Empty();
        Pool.ActiveOrderList->Empty();
        Pool.ActiveNodeMap->Empty();
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

    AActor* Spawned = GetReadyActor(Pool);

    if (Spawned)
    {
        Spawned->SetActorTransform(InTransform);

        UE_LOG(LogTemp, Log, TEXT("Spawn(Reuse) : %s"), *Spawned->GetName());
    }
    else
    {
        const bool bMax = Pool->IsFull();

        if (!bMax)
        {
            Spawned = CreateNewObject(InClassType, InTransform);

            UE_LOG(LogTemp, Log, TEXT("Spawn(New) : %s"), *Spawned->GetName());
        }
        else
        {
            switch (Pool->MaxPolicy)
            {
                case EObjectPoolPolicy::DoNotSpawn:
                    return nullptr;
                case EObjectPoolPolicy::Grow:
                    Spawned = CreateNewObject(InClassType, InTransform);

                    UE_LOG(LogTemp, Log, TEXT("Spawn(Grow) : %s"), *Spawned->GetName());
                    break;
                case EObjectPoolPolicy::ReuseOldest:
                    if (FOrderNode* Head = Pool->ActiveOrderList->GetHead())
                    {
                        AActor* OldestActor = Head->GetValue();
                        ReturnPool(OldestActor);

                        Spawned = GetReadyActor(Pool);
                        Spawned->SetActorTransform(InTransform);

                        UE_LOG(LogTemp, Log, TEXT("Spawn(ReuseOldest) : %s"), *Spawned->GetName());
                    }
                    break;
            }
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

        Pool->ActiveActors.Add(Spawned);
        FOrderNode* NewNode = new FOrderNode(Spawned);
        Pool->ActiveOrderList->AddTail(NewNode);
        Pool->ActiveNodeMap->Add(Spawned, NewNode);
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

    if (!Pool || !Pool->ActiveActors.Contains(InActor))
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
        //InActor->DisableComponentsSimulatePhysics();
    }

    Pool->ActiveActors.Remove(InActor);
    Pool->ReadyActors.Add(InActor);

    if (FOrderNode** FoundNode = Pool->ActiveNodeMap->Find(InActor))
    {
        Pool->ActiveOrderList->RemoveNode(*FoundNode, true);
        Pool->ActiveNodeMap->Remove(InActor);
    }

    UE_LOG(LogTemp, Log, TEXT("ReturnPool() : %s"), *InActor->GetName());
}

AActor* UObjectPoolSubsystem::CreateNewObject(TSubclassOf<AActor> InClassType, const FTransform& InTransform)
{
    if (!GetWorld())
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnParam;
    SpawnParam.Owner = nullptr;
    SpawnParam.ObjectFlags = RF_Transient;

    AActor* Spawned = GetWorld()->SpawnActor<AActor>(InClassType, InTransform, SpawnParam);

#if WITH_EDITOR
    if (Spawned)
    {
        Spawned->SetFolderPath(FName("Pool"));
    }
#endif

    return Spawned;
}

AActor* UObjectPoolSubsystem::GetReadyActor(FObjectPool* InPool)
{
    if (!InPool)
    {
        return nullptr;
    }

    AActor* ReadyActor = nullptr;
    while (InPool->ReadyActors.Num() > 0)
    {
        AActor* Candidate = InPool->ReadyActors.Pop();
        if (IsValid(Candidate))
        {
            ReadyActor = Candidate;
            break;
        }
    }

    return ReadyActor;
}
