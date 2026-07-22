// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "../Interface/StaminaInterface.h"

#include "ActionCharacter.generated.h"


// 전방 선언 모음
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UAnimInstance;
class UAnimMontage;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AActionCharacter : public ACharacter, public IStaminaInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AActionCharacter();

    virtual float GetCurrentStamina_Implementation() const override;
    virtual bool ConsumeStamina_Implementation(float InAmount) override;
    virtual void RecoveryStamina_Implementation(float InAmount) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    // IA_Test 키를 입력했을 때 호출할 함수. 반환 형식이랑 파라미터 형식 맞춰야됨
    void OnTestAction(const FInputActionValue& InValue);
    void OnMoveAction(const FInputActionValue& InValue);
    void OnRollAction(const FInputActionValue& InValue);
    void OnStartDashAction(const FInputActionValue& InValue);
    void OnEndDashAction(const FInputActionValue& InValue);

    void OnSprintStart();
    void OnSprintEnd();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Test = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Move = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Dash = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Roll = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TWeakObjectPtr<UAnimMontage> RollMontage = nullptr;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float WalkSpeed = 0.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float CurrentStamina = 100.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float StaminaRecoveryTime = 3.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float StaminaRecoveryAmount = 10.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float RollStamina = 30.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float DashStamina = 20.0f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> CameraComponent = nullptr;

private:
    UPROPERTY()
    TObjectPtr<UAnimInstance> AnimInstance = nullptr;

    bool bIsDashing = false;
    float StaminaElapsedTime = 0.0f;
};
