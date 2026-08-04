// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Interface/PoolableInterface.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Containers/Map.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 프로젝트 세팅에서 데이터 읽어오기
    const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();
    if (Settings)
    {
        //if (!Settings->DamagePopupClass.IsNull())
        //{
        //    DamagePopupClass = Settings->DamagePopupClass.LoadSynchronous();
        //}

        for (auto& PoolableClass : Settings->PoolableClasses)
        {
            if (!PoolableClass.IsNull())
            {
                TSubclassOf<AActor> LoadedClass = PoolableClass.LoadSynchronous();
                GenericPoolMap.Add(LoadedClass, FObjectPool(LoadedClass));
                UE_LOG(LogTemp, Display, TEXT("GenericPoolMap에 %s 풀 추가"), *LoadedClass->GetName());
            }
        }
    }
}

AActor* UObjectPoolSubsystem::Spawn(const FTransform& InTransform)
{
    /*
    AActor* Spawned = nullptr;
    if (ReadyActors.Num() > 0)
    {
        Spawned = ReadyActors.Pop(); // 뒤에서부터 꺼내기
        Spawned->SetActorTransform(InTransform);
        UE_LOG(LogTemp, Display, TEXT("ObjectPoolSubsystem.Spawn() - 뒤에서부터 꺼내기"));
    }
    else
    {
        if (DamagePopupClass && GetWorld())
        {
            FActorSpawnParameters SpawnParam;
            SpawnParam.Owner = nullptr;
            SpawnParam.ObjectFlags = RF_Transient;

            Spawned = GetWorld()->SpawnActor<AActor>(DamagePopupClass, InTransform, SpawnParam);
#if WITH_EDITOR
            if (Spawned)
            {
                Spawned->SetFolderPath(FName("Pool"));
                UE_LOG(LogTemp, Display, TEXT("ObjectPoolSubsystem.Spawn() - Pool 폴더에 새로 만들기"));
            }
#endif
        }
    }

    UE_LOG(LogTemp, Display, TEXT("1"));
    if (Spawned)
    {
        if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
        {
            IPoolableInterface::Execute_OnSpawn(Spawned);
        }

        ActiveActors.Add(Spawned);
    }

    return Spawned;
    */

    AActor* Spawned = nullptr;

    if (ReadyActors.Num() > 0)
    {
        Spawned = ReadyActors.Pop(); // 뒤에서부터 꺼내기
        Spawned->SetActorTransform(InTransform);
        UE_LOG(LogTemp, Display, TEXT("ObjectPoolSubsystem.Spawn() - 뒤에서부터 꺼내기"));
    }
    else
    {
        if (DamagePopupClass && GetWorld())
        {
            FActorSpawnParameters SpawnParam;
            SpawnParam.Owner = nullptr;
            SpawnParam.ObjectFlags = RF_Transient;

            Spawned = GetWorld()->SpawnActor<AActor>(DamagePopupClass, InTransform, SpawnParam);
#if WITH_EDITOR
            if (Spawned)
            {
                Spawned->SetFolderPath(FName("Pool"));
                UE_LOG(LogTemp, Display, TEXT("ObjectPoolSubsystem.Spawn() - Pool 폴더에 새로 만들기"));
            }
#endif
        }
    }

    UE_LOG(LogTemp, Display, TEXT("1"));
    if (Spawned)
    {
        if (Spawned->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
        {
            IPoolableInterface::Execute_OnSpawn(Spawned);
        }

        ActiveActors.Add(Spawned);
    }

    return Spawned;
}

AActor* UObjectPoolSubsystem::Spawn(TSubclassOf<AActor> InActorClass, const FTransform& InTransform)
{
    AActor* Spawned = nullptr;

    if (!GenericPoolMap.Contains(InActorClass))
    {
        return nullptr;
    }

    FObjectPool& ObjectPool = GenericPoolMap[InActorClass];

    if (ObjectPool.ReadyActors.Num() > 0)
    {
        Spawned = ObjectPool.ReadyActors.Pop();
        Spawned->SetActorTransform(InTransform);
        UE_LOG(LogTemp, Display, TEXT("Spawn() : Reuse %s"), *Spawned->GetName());
    }
    else
    {
        if (ObjectPool.ObjectClass && GetWorld())
        {
            FActorSpawnParameters SpawnParam;
            SpawnParam.Owner = nullptr;
            SpawnParam.ObjectFlags = RF_Transient;

            Spawned = GetWorld()->SpawnActor<AActor>(ObjectPool.ObjectClass, InTransform, SpawnParam);
#if WITH_EDITOR
            if (Spawned)
            {
                Spawned->SetFolderPath(FName("Pool"));
                UE_LOG(LogTemp, Display, TEXT("Spawn() : Create %s"), *Spawned->GetName());
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

        ObjectPool.ActiveActors.Add(Spawned);
    }

    return Spawned;
}

void UObjectPoolSubsystem::ReturnPool(AActor* InActor)
{
    //UE_LOG(LogTemp, Display, TEXT("ObjectPoolSubsystem.ReturnPool()"));
    if (!InActor)
    {
        return;
    }

    if (!ActiveActors.Contains(InActor))
    {
        return;
    }

    if (InActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
    {
        IPoolableInterface::Execute_OnReturn(InActor);
    }

    ActiveActors.Remove(InActor);
    ReadyActors.Add(InActor);
    UE_LOG(LogTemp, Display, TEXT("ObjectPoolSubsystem.ReturnPool() - 되돌리기"));
}

void UObjectPoolSubsystem::ReturnPool(TSubclassOf<AActor> InActorClass, AActor* InActor)
{
    if (!InActor)
    {
        return;
    }

    if (!GenericPoolMap.Contains(InActorClass))
    {
        return;
    }

    FObjectPool& ObjectPool = GenericPoolMap[InActorClass];

    if (!ObjectPool.ActiveActors.Contains(InActor))
    {
        return;
    }

    if (InActor->GetClass()->ImplementsInterface(UPoolableInterface::StaticClass()))
    {
        IPoolableInterface::Execute_OnReturn(InActor);
    }

    ObjectPool.ActiveActors.Remove(InActor);
    ObjectPool.ReadyActors.Add(InActor);

    UE_LOG(LogTemp, Display, TEXT("ReturnPool() : %s"), *InActor->GetName());
}
