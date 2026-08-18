// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Interface/StatInterface.h"
#include "Interface/WeaponUserInterface.h"
#include "Interface/InventoryUserInterface.h"

#include "ActionCharacter.generated.h"

// 전방 선언 모음
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UWeaponComponent;
class UInventoryComponent;
class UAnimNotifyState_SectionJump;
class UAnimMontage;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AActionCharacter : public ACharacter, public IStatInterface, public IWeaponUserInterface, public IInventoryUserInterface
{
    GENERATED_BODY()

public:
    AActionCharacter();

    virtual void EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData) override;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual bool ExecuteInventoryCommand(const FInventoryCommand& Command, FInventoryCommandResult& OutResult) override;

    /* Getter, Setter */
    UFUNCTION(BlueprintCallable, Category = "Stat")
    virtual UStatComponent* GetStatComponent() const override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    virtual UWeaponComponent* GetWeaponComponent() const override;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    virtual UInventoryComponent* GetInventoryComponent() const override;

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
    void OnInventoryAction(const FInputActionValue& InValue);

private:
    void SpendSprintStamina(float DeltaTime);

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Inventory = nullptr;

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

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> CameraComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStatComponent> StatComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UWeaponComponent> WeaponComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;

private:
    UPROPERTY()
    TObjectPtr<UAnimInstance> AnimInstance = nullptr;

    bool bSprintMode = false;

};
