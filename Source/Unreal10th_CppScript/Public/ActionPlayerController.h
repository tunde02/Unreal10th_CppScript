// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "ActionPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AActionPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UInputAction> IA_Look = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ViewPitchMin = -40.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float ViewPitchMax = 30.0f;

private:
    void OnLookInput(const FInputActionValue& InValue);

private:
    int32 GameInputPriority = 1;
};
