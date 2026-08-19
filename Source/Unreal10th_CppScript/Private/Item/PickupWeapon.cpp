// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupWeapon.h"

#include "Weapon/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"
#include "Component/WeaponComponent.h"
#include "Data/Item/WeaponDataAsset.h"

#include "Components/SphereComponent.h"
#include "Curves/CurveFloat.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

APickupWeapon::APickupWeapon()
{
    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionProfileName("NoCollision");
}

void APickupWeapon::InitializePickup(UItemDataAsset* InData)
{
    Super::InitializePickup(InData);

    if (DataAsset)
    {
        WeaponData = Cast<UWeaponDataAsset>(DataAsset);

        if (USkeletalMesh* SkeletalMeshData = WeaponData->Mesh.LoadSynchronous())
        {
            Mesh->SetSkeletalMesh(SkeletalMeshData);
            Mesh->SetRelativeLocation(MeshBaseLocation + WeaponData->SpawnLocationOffset);
        }
    }
}

void APickupWeapon::BeginPlay()
{
    Super::BeginPlay();

    Mesh->SetGenerateOverlapEvents(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

UMeshComponent* APickupWeapon::GetMesh() const
{
    return Mesh;
}
