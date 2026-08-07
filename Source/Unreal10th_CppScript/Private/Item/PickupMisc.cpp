// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupMisc.h"
#include "Data/Item/MiscItemDataAsset.h"

APickupMisc::APickupMisc()
{
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
    Mesh->SetCollisionProfileName("NoCollision");
}

void APickupMisc::InitializePickup(UItemDataAsset* InData)
{
    Super::InitializePickup(InData);

    if (DataAsset)
    {
        MiscData = Cast<UMiscItemDataAsset>(DataAsset);

        if (UStaticMesh* StaticMeshData = MiscData->Mesh.LoadSynchronous())
        {
            Mesh->SetStaticMesh(StaticMeshData);
            Mesh->SetRelativeLocation(MeshBaseLocation + MiscData->SpawnLocationOffset);
        }
    }
}

void APickupMisc::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    InitializePickup(DataAsset);
}

UMeshComponent* APickupMisc::GetMesh() const
{
    return Mesh;
}
