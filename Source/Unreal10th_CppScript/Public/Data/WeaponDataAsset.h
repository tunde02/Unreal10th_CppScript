// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StreamableManager.h"
#include "WeaponDataAsset.generated.h"

class UStaticMesh;
class UNiagaraSystem;

/**
 *
 */
UCLASS(BlueprintType)
class UNREAL10TH_CPPSCRIPT_API UWeaponDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    TSharedPtr<FStreamableHandle> RequestDataLoad(FStreamableDelegate InDelegate);
    bool IsLoaded() const;

public:
    // 무기 메시
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
    TSoftObjectPtr<UStaticMesh> Mesh;

    // 무기가 부착될 소켓 이름
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
    FName AttachSocketName = TEXT("hand_rSocket");

    // 무기를 부착할 위치의 오프셋
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
    FVector LocationOffset = FVector::ZeroVector;

    // HitArea 캡슐의 높이 절반
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
    float HitAreaHalfHeight = 60.0f;

    // HitArea 캡슐의 반지름
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HitArea")
    float HitAreaRadius = 30.0f;

    // 무기 공격력
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    float AttackPower = 20.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    int32 UseCount = 3;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    TSoftObjectPtr<UNiagaraSystem> WeaponHitVfx;

};
