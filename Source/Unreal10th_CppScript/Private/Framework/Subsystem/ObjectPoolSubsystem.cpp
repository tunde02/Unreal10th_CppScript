// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Config/ObjectPoolSettings.h"
#include "Interface/PoolableInterface.h"

void UObjectPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 프로젝트 세팅에서 데이터 읽어오기
    const UObjectPoolSettings* Settings = GetDefault<UObjectPoolSettings>();
    if (Settings)
    {
        if (!Settings->DamagePopupClass.IsNull())
        {
            DamagePopupClass = Settings->DamagePopupClass.LoadSynchronous();
        }
    }
}

AActor* UObjectPoolSubsystem::Spawn(const FTransform& InTransform)
{
    //UE_LOG(LogTemp, Display, TEXT("ObjectPoolSubsystem.Spawn()"));
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
