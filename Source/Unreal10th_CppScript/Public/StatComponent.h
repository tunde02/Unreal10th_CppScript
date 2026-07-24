// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Interface/StaminaInterface.h"
#include "../Interface/HealthInterface.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatEmpty);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChange, float, Current, float, Max);

struct FAutoRecoveryData
{
    float CoolTime = 3.0f;
    float TickInterval = 0.1f;
    float RecoveryPerTick = 1.0f;

    FAutoRecoveryData() : CoolTime(1.0f), TickInterval(1.0f), RecoveryPerTick(1.0f)
    {
    }
    FAutoRecoveryData(float InCoolTime, float InTickInterval, float InRecoveryPerTick)
        : CoolTime(InCoolTime), TickInterval(InTickInterval), RecoveryPerTick(InRecoveryPerTick)
    {
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UNREAL10TH_CPPSCRIPT_API UStatComponent : public UActorComponent, public IStaminaInterface, public IHealthInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UStatComponent();

    void Initialize(FAutoRecoveryData& InData);

    virtual float GetCurrentStamina_Implementation() const override;
    virtual float GetMaxStamina_Implementation() const override;
    virtual bool ConsumeStamina_Implementation(float InAmount) override;
    virtual void RecoveryStamina_Implementation(float InAmount) override;

    virtual float GetCurrentHealth_Implementation() const override;
    virtual float GetMaxHealth_Implementation() const override;
    virtual void DamageHealth_Implementation(float InAmount) override;
    virtual void HealHealth_Implementation(float InAmount) override;

public:
    UPROPERTY(BlueprintAssignable, Category = "Stat|Stamina")
    FOnStatEmpty OnStaminaEmpty;

    UPROPERTY(BlueprintAssignable, Category = "Stat|Health")
    FOnStatEmpty OnDie;

    UPROPERTY(BlueprintAssignable, Category = "Stat|Stamina")
    FOnStatChange OnStaminaChange;

    UPROPERTY(BlueprintAssignable, Category = "Stat|Health")
    FOnStatChange OnHealthChange;

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // DEPRECATED : 타이머로 변경하면서 더 이상 사용하지 않음
    //void StaminaAutoRecovery(float DeltaTime);

private:
    void StaminaAutoRecovery();

protected:
    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Stamina")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Stamina")
    float CurrentStamina = 100.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Health")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Stat|Health")
    float CurrentHealth = 100.0f;

private:
    //float StaminaAutoRecoverySeconds = 0.0f; // DEPRECATED : 타이머로 변경하면서 더 이상 사용하지 않음
    FTimerHandle StaminaAutoRecoveryTimerHandle;

    FAutoRecoveryData AutoRecoveryData;

    const float StaminaEmptyCheckLimit = 0.01f;
};
