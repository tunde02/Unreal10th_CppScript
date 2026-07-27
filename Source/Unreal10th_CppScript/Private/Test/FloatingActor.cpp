// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/FloatingActor.h"

// Sets default values
AFloatingActor::AFloatingActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
    MainMesh->SetupAttachment(RootComponent);
}

void AFloatingActor::FloatingByCos(UStaticMeshComponent* InStaticMesh, float InDeltaTime)
{
    CurrentDegree += InDeltaTime * 360.0f / Period;
    float Radian = FMath::DegreesToRadians(CurrentDegree);

    InStaticMesh->SetRelativeLocation(FVector(0, 0, FMath::Cos(Radian) * Distance));
}

// Called when the game starts or when spawned
void AFloatingActor::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void AFloatingActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

