// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"
#include "Component/StatComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interface/StatInterface.h"
#include "Interface/HealthInterface.h"

// Sets default values
AHealthPickup::AHealthPickup()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
    SphereCollision->InitSphereRadius(100.0f);
    SetRootComponent(SphereCollision);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHealthPickup::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void AHealthPickup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AHealthPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    if (!IsValid(OtherActor) || !OtherActor->Implements<UStatInterface>())
    {
        return;
    }

    UStatComponent* StatComponent = Cast<IStatInterface>(OtherActor)->GetStatComponent();

    if (!IsValid(StatComponent) || !StatComponent->Implements<UHealthInterface>())
    {
        return;
    }

    if (HealthAmount > 0.0f)
    {
        IHealthInterface::Execute_HealHealth(StatComponent, HealthAmount);
    }
    else
    {
        IHealthInterface::Execute_DamageHealth(StatComponent, -HealthAmount);
    }
}