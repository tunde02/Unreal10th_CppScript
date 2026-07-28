// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/WeaponPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Weapon/WeaponActor.h"

// Sets default values
AWeaponPickup::AWeaponPickup()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
    SetRootComponent(SphereCollision);
    SphereCollision->InitSphereRadius(100.0f);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(SphereCollision);
}

// Called when the game starts or when spawned
void AWeaponPickup::BeginPlay()
{
    Super::BeginPlay();

}

void AWeaponPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AWeaponActor* NewWeapon = GetWorld()->SpawnActor<AWeaponActor>(WeaponActorClass, SpawnParams);

    NewWeapon->OnEquipped(OtherActor);
}
