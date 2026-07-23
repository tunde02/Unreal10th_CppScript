// Fill out your copyright notice in the Description page of Project Settings.


#include "TestTimer.h"

// Sets default values
ATestTimer::ATestTimer()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestTimer::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ATestTimer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ATestTimer::Test_TimerOnce()
{
    UE_LOG(LogTemp, Log, TEXT("Test_TimerOnce : Start"));

    UWorld* World = GetWorld();
    FTimerManager& TimerManager = World->GetTimerManager();

    TimerManager.SetTimer(
        TestOnce,   // TestOnce가 핸들인 타이머
        this,       // 이 객체의
        &ATestTimer::Test_MessagPrint1, // 이 함수를
        3.0f,       // 3초 간격으로 실행
        false       // 반복하지 않는다
    );
}

void ATestTimer::Test_TimerRepeat()
{
    UE_LOG(LogTemp, Log, TEXT("Test_TimerRepeat : Start"));

    UWorld* World = GetWorld();
    FTimerManager& TimerManager = World->GetTimerManager();

    TimerManager.SetTimer(
        TestRepeat,
        this,
        &ATestTimer::Test_MessagPrint1,
        3.0f,
        true // 계속 반복한다.
    );
}

void ATestTimer::Test_TimerDelayRepeat()
{
    UE_LOG(LogTemp, Log, TEXT("Test_TimerDelayRepeat : Start"));

    UWorld* World = GetWorld();
    FTimerManager& TimerManager = World->GetTimerManager();

    TimerManager.SetTimer(
        TestDelayRepeat,
        this,
        &ATestTimer::Test_MessagPrint1,
        3.0f,
        true,
        5.0f // 처음에 5초 기다린 후 실행
    );
}

void ATestTimer::Test_TimerLambda()
{
    UE_LOG(LogTemp, Log, TEXT("Test_TimerLambda : Start"));

    UWorld* World = GetWorld();
    FTimerManager& TimerManager = World->GetTimerManager();

    TimerManager.SetTimer(
        TestLambda,
        FTimerDelegate::CreateLambda(
            []() {
                UE_LOG(LogTemp, Log, TEXT("Test_TimerLambda : TimerInvoke"));
            }
        ),
        3.0f,
        true
    );
}

void ATestTimer::Test_TimerAllClear()
{
    UWorld* World = GetWorld();
    FTimerManager& TimerManager = World->GetTimerManager();

    TimerManager.ClearTimer(TestOnce);
    TimerManager.ClearTimer(TestRepeat);
    TimerManager.ClearTimer(TestDelayRepeat);
    TimerManager.ClearTimer(TestLambda);
}

void ATestTimer::Test_MessagPrint1()
{
    UE_LOG(LogTemp, Log, TEXT("Test_TimerOnce : TimerInvoke"));
}

void ATestTimer::Test_MessagPrint2()
{
    UE_LOG(LogTemp, Log, TEXT("Test_TimerRepeat : TimerInvoke"));
}

void ATestTimer::Test_MessagPrint3()
{
    UE_LOG(LogTemp, Log, TEXT("Test_TimeDelayRepeat : TimerInvoke"));
}

