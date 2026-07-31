// Fill out your copyright notice in the Description page of Project Settings.


#include "Deprecated/StaminaPickup.h"
#include "Component/StatComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interface/StatInterface.h"
#include "Interface/StaminaInterface.h"

// Sets default values
AStaminaPickup::AStaminaPickup()
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
void AStaminaPickup::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void AStaminaPickup::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void AStaminaPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    if (!IsValid(OtherActor) || !OtherActor->Implements<UStatInterface>())
    {
        return;
    }

    UStatComponent* StatComponent = Cast<IStatInterface>(OtherActor)->GetStatComponent();

    if (!IsValid(StatComponent) || !StatComponent->Implements<UStaminaInterface>())
    {
        return;
    }

    if (Stamina > 0.0f)
    {
        IStaminaInterface::Execute_RecoveryStamina(StatComponent, Stamina);
    }
    else
    {
        IStaminaInterface::Execute_ConsumeStamina(StatComponent, -Stamina);
    }
}
