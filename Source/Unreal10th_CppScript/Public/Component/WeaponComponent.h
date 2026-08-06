// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWeaponAttackStateChanged, bool, bEnable);

class UWeaponDataAsset;
class AWeaponActor;
class UAnimNotifyState_SectionJump;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREAL10TH_CPPSCRIPT_API UWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UWeaponComponent();

    bool Attack();

    void AreaAttack();

    void Initialize();

    UFUNCTION(BlueprintCallable)
    void EquipWeapon(UWeaponDataAsset* InWeaponData);

    // 무기가 드랍되었을 때 실행될 함수 (WeaponActor 에서 바인딩)
    void OnWeaponDrop(UWeaponDataAsset* InDropWeaponData);

    // 무기 공격 활성화/비활성화 때 실행될 함수
    void OnWeaponAttackState(bool bEnable);

    bool CanWeaponUse() const;
    bool IsComboReady() const { return bComboReady; }

    void SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJumpNotify);

    /* Getter, Setter */
    FOnWeaponAttackStateChanged& GetWeaponAttackStateChangedDelegate() { return OnWeaponAttackStateChanged; }
    void SetComboReady(bool bInComboReady) { bComboReady = bInComboReady; }

protected:
    virtual void BeginPlay() override;

private:
    void SpawnWeaponActorAndEquip();

    bool SectionJumpForCombo();

    // 공격 몽타주가 끝날 때 실행될 함수
    void OnAttackEnded(UAnimMontage* InMontage, bool bInterrupted);

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
    TWeakObjectPtr<UAnimNotifyState_SectionJump> SectionJumpNotify = nullptr; // 발생한 콤보 노티파이를 저장해 놓는 변수

    bool bComboReady = false;

    TWeakObjectPtr<ACharacter> OwnerCharacter = nullptr; // 이 컴포넌트가 부착된 캐릭터

    TWeakObjectPtr<UAnimInstance> OwnerAnimInstance = nullptr; // OwnerCharacter의 AnimInstance

};
