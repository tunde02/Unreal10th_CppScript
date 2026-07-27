// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestTimer.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API ATestTimer : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ATestTimer();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    void Test_TimerOnce();

    UFUNCTION(BlueprintCallable)
    void Test_TimerRepeat();

    UFUNCTION(BlueprintCallable)
    void Test_TimerDelayRepeat();

    UFUNCTION(BlueprintCallable)
    void Test_TimerLambda();

    UFUNCTION(BlueprintCallable)
    void Test_TimerAllClear();

private:
    void Test_MessagPrint1();
    void Test_MessagPrint2();
    void Test_MessagPrint3();

private:
    FTimerHandle TestOnce;
    FTimerHandle TestRepeat;
    FTimerHandle TestDelayRepeat;
    FTimerHandle TestLambda;
};
