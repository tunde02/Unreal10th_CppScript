// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/ItemDataAsset.h"
#include "Data/Item/Action/ItemAction.h"
#include "NiagaraSystem.h"
#include "WeaponDataAsset.generated.h"

class USkeletalMesh;

UCLASS(BlueprintType)
class UNREAL10TH_CPPSCRIPT_API UWeaponDataAsset : public UItemDataAsset
{
    GENERATED_BODY()

public:
    virtual bool IsLoaded() const override;

protected:
    virtual void OnAsyncRequest(TArray<FSoftObjectPath>& InOutTargetsToLoad) const override;

public:
    // 무기 메시
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
    TSoftObjectPtr<USkeletalMesh> Mesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
    TSoftObjectPtr<UNiagaraSystem> TrailVfx;

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

    // 무기의 범위 공격력
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    float AreaAttackPower = 20.0f;

    // 범위 공격의 안쪽 반지름
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    float AreaAttackInnerRadius = 100.0f;

    // 범위 공격의 바깥쪽 반지름
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    float AreaAttackOuterRadius = 300.0f;

    // 무기 사용 횟수
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    int32 UseCount = 3;

    // 무기 사용 횟수 무한 여부
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    bool bInfinityUse = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    TSoftObjectPtr<UNiagaraSystem> WeaponHitVfx;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
    TObjectPtr<UAnimMontage> AttackMontage = nullptr;

    UPROPERTY(EditAnywhere, Category = "Action")
    TObjectPtr<UItemAction> ItemAction = nullptr;

};
