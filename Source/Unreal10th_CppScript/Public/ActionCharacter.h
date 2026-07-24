// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "../Interface/StatInterface.h"

#include "ActionCharacter.generated.h"


// 전방 선언 모음
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UStatComponent;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AActionCharacter : public ACharacter, public IStatInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AActionCharacter();

    UFUNCTION(BlueprintCallable, Category="Stat")
    virtual UStatComponent* GetStatComponent() const override;

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
    void OnSprintStart();
    void OnSprintEnd();

private:
    void SpendSprintStamina(float DeltaTime);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Test = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Move = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Sprint = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Roll = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UAnimMontage> RollMontage = nullptr;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite)
    float WalkSpeed = 0.0f;

    // 구르기에 필요한 스태미나 코스트
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category="Stat|Stamina")
    float RollStaminaCost = 30.0f;

    // 달리기에 필요한 초당 스태미나 코스트
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category="Stat|Stamina")
    float SprintStaminaCostPerSecond = 5.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category="Stat|Stamina")
    float StaminaAutoRecoveryCoolTime = 3.0f;

    // DEPRECATED : 타이머로 변경하면서 더 이상 사용하지 않음
    //UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category="Stat|Stamina")
    //float StaminaAutoRecoveryAmountPerSecond = 10.0f;

    // 스태미나 자동 회복 타이머 틱당 회복량
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category="Stat|Stamina")
    float StaminaAutoRecoveryAmountPerTick = 1.0f;

    // 스태미나 자동 회복 타이머 인터벌
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category="Stat|Stamina")
    float StaminaAutoRecoveryInterval = 0.1f;

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
};
