// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Pickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Component/StatComponent.h"
#include "Interface/StatInterface.h"
#include "Interface/StaminaInterface.h"
#include "Interface/HealthInterface.h"

// Sets default values
APickup::APickup()
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
void APickup::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void APickup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void APickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    ApplyEffects(OtherActor);
}

void APickup::ApplyEffects(AActor* InTarget)
{
    if (IStatInterface* Stat = Cast<IStatInterface>(InTarget))
    {
        UStatComponent* StatComponent = Stat->GetStatComponent();

        if (Stamina > 0.0f)
        {
            IStaminaInterface::Execute_RecoveryStamina(StatComponent, Stamina);
        }
        else if (Stamina < 0.0f)
        {
            IStaminaInterface::Execute_ConsumeStamina(StatComponent, -Stamina);
        }

        if (Health > 0.0f)
        {
            IHealthInterface::Execute_HealHealth(StatComponent, Health);
        }
        else if (Health < 0.0f)
        {
            IHealthInterface::Execute_DamageHealth(StatComponent, -Health);
        }
    }
}
