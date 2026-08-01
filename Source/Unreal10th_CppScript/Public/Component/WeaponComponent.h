// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWeaponAttackStateChanged, bool, bEnable);

class UWeaponDataAsset;
class AWeaponActor;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREAL10TH_CPPSCRIPT_API UWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWeaponComponent();

    void Initialize();

    UFUNCTION(BlueprintCallable)
    void EquipWeapon(UWeaponDataAsset* InWeaponData);

    // 무기가 드랍되었을 때 실행될 함수
    void OnWeaponDrop(UWeaponDataAsset* InDropWeaponData);

    // 무기 공격 활성화/비활성화 때 실행될 함수
    void OnWeaponAttackState(bool bEnable) const;

    bool IsCurrentWeaponValid() const;
    bool CanCurrentWeaponUse() const;
    bool IsComboReady() const { return bComboReady; }

    // 공격 몽타주가 끝날 때 실행될 함수
    void OnAttackEnded(UAnimMontage* InMontage, bool bInterrupted);

    /* Getter, Setter */
    FOnWeaponAttackStateChanged& GetWeaponAttackStateChangedDelegate() { return OnWeaponAttackStateChanged; }
    void SetComboReady(bool bInComboReady) { bComboReady = bInComboReady; }

private:
    void SpawnWeaponActorAndEquip();

public:
    FOnWeaponAttackStateChanged OnWeaponAttackStateChanged;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TObjectPtr<UWeaponDataAsset> DefaultWeaponData = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UWeaponDataAsset> CurrentWeaponData = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TWeakObjectPtr<AWeaponActor> CurrentWeapon = nullptr;

private:
    bool bComboReady = false;
};
