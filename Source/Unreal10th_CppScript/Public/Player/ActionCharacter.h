// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interface/StatInterface.h"
#include "Interface/WeaponUserInterface.h"

#include "ActionCharacter.generated.h"

// 전방 선언 모음
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UAnimNotifyState_SectionJump;
class AWeaponActor;
class UWeaponDataAsset;
class UAnimMontage;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AActionCharacter : public ACharacter, public IStatInterface, public IWeaponUserInterface
{
    GENERATED_BODY()

public:
    AActionCharacter();

    /* 무기 장비 관련 함수들 */
    virtual void EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData) override;

    // 무기가 드랍되었을 때 실행될 함수
    void OnWeaponDrop(UWeaponDataAsset* InDropWeaponData);

    /* 이벤트 함수 */
    // 무기 공격 활성화/비활성화 때 실행될 함수
    virtual void OnWeaponAttackState(bool bEnable) override;

    /* Getter, Setter */
    UFUNCTION(BlueprintCallable, Category = "Stat")
    virtual UStatComponent* GetStatComponent() const override;

    virtual inline FOnWeaponAttackStateChanged& GetWeaponAttackStateChangedDelegate() { return OnWeaponAttackStateChanged; }

    void SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJumpNotify);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
    void OnTestAction(const FInputActionValue& InValue);
    void OnMoveAction(const FInputActionValue& InValue);
    void OnAttackAction(const FInputActionValue& InValue);
    void OnRollAction(const FInputActionValue& InValue);
    void OnSprintStart();
    void OnSprintEnd();

private:
    void SpendSprintStamina(float DeltaTime);

    void SectionJumpForCombo();

    void SpawnWeaponActorAndEquip();

    // 공격 몽타주가 끝날 때 실행될 함수
    void OnAttackEnded(UAnimMontage* InMontage, bool bInterrupted);

public:
    FOnWeaponAttackStateChanged OnWeaponAttackStateChanged;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Test = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Move = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Attack = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Roll = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Sprint = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Anims")
    TObjectPtr<UAnimMontage> AttackMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Anims")
    TObjectPtr<UAnimMontage> RollMontage = nullptr;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Move")
    float WalkSpeed = 0.0f;

    // 구르기에 필요한 스태미나 코스트
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Stamina")
    float RollStaminaCost = 30.0f;

    // 달리기에 필요한 초당 스태미나 코스트
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Stamina")
    float SprintStaminaCostPerSecond = 5.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Stamina")
    float StaminaAutoRecoveryCoolTime = 3.0f;

    // 스태미나 자동 회복 타이머 틱당 회복량
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Stamina")
    float StaminaAutoRecoveryAmountPerTick = 1.0f;

    // 스태미나 자동 회복 타이머 인터벌
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Stamina")
    float StaminaAutoRecoveryInterval = 0.1f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Attack")
    float AttackCost = 5.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TWeakObjectPtr<AWeaponActor> CurrentWeapon = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UWeaponDataAsset> CurrentWeaponData = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    TObjectPtr<UWeaponDataAsset> DefaultWeaponData = nullptr;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> CameraComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStatComponent> StatComponent = nullptr;

private:
    UPROPERTY()
    TObjectPtr<UAnimInstance> AnimInstance = nullptr;

    bool bSprintMode = false;

    TWeakObjectPtr<UAnimNotifyState_SectionJump> SectionJumpNotify = nullptr; // 발생한 콤보 노티파이를 저장해 놓는 변수

    bool bComboReady = false; // 현재 콤보가 가능한지 확인하는 함수

};
