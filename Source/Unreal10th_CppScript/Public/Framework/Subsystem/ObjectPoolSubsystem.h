// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ObjectPoolSubsystem.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UObjectPoolSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection);
    AActor* Spawn(const FTransform& InTransform);
    void ReturnPool(AActor* InActor);

    //template<typename T>
    //T MySpawn(const FTransform& InTransform);

protected:
    // 사용 대기 중인 액터들
    UPROPERTY(Transient)
    TArray<TObjectPtr<AActor>> ReadyActors;

    // 실제 사용 중인 액터들
    UPROPERTY(Transient)
    TSet<TObjectPtr<AActor>> ActiveActors;

    UPROPERTY()
    TSubclassOf<AActor> DamagePopupClass = nullptr;

    //UPROPERTY(Transient)
    //TMap<TSubclassOf<AActor>, TObjectPtr<AActor>> MyPool;

};
//
//template<typename T>
//inline T UObjectPoolSubsystem::MySpawn(const FTransform& InTransform)
//{
//
//    return MyPool[T];
//}
