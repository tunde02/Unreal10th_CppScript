// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"

#include "ActionCharacter.generated.h"


// 전방 선언 모음
class UInputAction;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AActionCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AActionCharacter();

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
    void OnStartDashAction(const FInputActionValue& InValue);
    void OnEndDashAction(const FInputActionValue& InValue);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Test = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Move = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Dash = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float WalkSpeed = 0.0f;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USpringArmComponent> CameraSpringArmComponent = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UCameraComponent> CameraComponent = nullptr;
};
