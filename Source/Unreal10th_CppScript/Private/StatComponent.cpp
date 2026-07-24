// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

void UStatComponent::Initialize(FAutoRecoveryData& InData)
{
    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;
    AutoRecoveryData = InData;

    OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
    OnStaminaChange.Broadcast(CurrentStamina, MaxStamina);
}

float UStatComponent::GetCurrentStamina_Implementation() const
{
    return CurrentStamina;
}

float UStatComponent::GetMaxStamina_Implementation() const
{
    return MaxStamina;
}

bool UStatComponent::ConsumeStamina_Implementation(float InAmount)
{
    bool bResult = false;

    if (CurrentStamina >= InAmount)
    {
        CurrentStamina -= InAmount;

        FTimerManager& TimerManager = GetWorld()->GetTimerManager();
        TimerManager.SetTimer(
            StaminaAutoRecoveryTimerHandle,
            this,
            &UStatComponent::StaminaAutoRecovery,
            AutoRecoveryData.TickInterval,
            true,
            AutoRecoveryData.CoolTime
        );

        OnStaminaChange.Broadcast(CurrentStamina, MaxStamina);

        if (CurrentStamina < StaminaEmptyCheckLimit)
        {
            OnStaminaEmpty.Broadcast();
        }

        bResult = true;
    }

    //UE_LOG(LogTemp, Log, TEXT("Stamina : %.1f / %.1f"), CurrentStamina, MaxStamina);

    return bResult;
}

void UStatComponent::RecoveryStamina_Implementation(float InAmount)
{
    CurrentStamina = FMath::Clamp(CurrentStamina + InAmount, 0.0f, MaxStamina);
    OnStaminaChange.Broadcast(CurrentStamina, MaxStamina);

    //UE_LOG(LogTemp, Log, TEXT("Stamina : %.1f / %.1f"), CurrentStamina, MaxStamina);

    // FMath::IsNearlyEqual(CurrentStamina, MaxStamina) 이걸로 비교해도 됨
    if (CurrentStamina >= MaxStamina)
    {
        FTimerManager& TimerManager = GetWorld()->GetTimerManager();
        TimerManager.ClearTimer(StaminaAutoRecoveryTimerHandle);
    }
}

float UStatComponent::GetCurrentHealth_Implementation() const
{
    return CurrentHealth;
}

float UStatComponent::GetMaxHealth_Implementation() const
{
    return MaxHealth;
}

void UStatComponent::DamageHealth_Implementation(float InAmount)
{
    CurrentHealth -= InAmount;
    if (CurrentHealth <= 0.0f)
    {
        CurrentHealth = 0.0f;
        OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
        OnDie.Broadcast();
    }
    else
    {
        OnHealthChange.Broadcast(CurrentHealth, MaxHealth);
    }

    //UE_LOG(LogTemp, Log, TEXT("Health : %.1f / %.1f"), CurrentHealth, MaxHealth);
}

void UStatComponent::HealHealth_Implementation(float InAmount)
{
    CurrentHealth = FMath::Min(CurrentHealth + InAmount, MaxHealth);
    OnHealthChange.Broadcast(CurrentHealth, MaxHealth);

    //UE_LOG(LogTemp, Log, TEXT("Health : %.1f / %.1f"), CurrentHealth, MaxHealth);
}

// Called when the game starts
void UStatComponent::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

void UStatComponent::StaminaAutoRecovery()
{
    IStaminaInterface::Execute_RecoveryStamina(this, AutoRecoveryData.RecoveryPerTick);
}

// DEPRECATED: 타이머로 대체
//void UStatComponent::StaminaAutoRecovery(float DeltaTime)
//{
//    if (StaminaAutoRecoverySeconds < 0.0f && CurrentStamina < MaxStamina)
//    {
//        IStaminaInterface::Execute_RecoveryStamina(this, StaminaAutoRecoveryAmountPerSecond * DeltaTime);
//    }
//    else
//    {
//        StaminaAutoRecoverySeconds -= DeltaTime;
//    }
//}

