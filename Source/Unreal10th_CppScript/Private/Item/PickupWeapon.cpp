// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupWeapon.h"
#include "Weapon/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"
#include "Data/WeaponDataAsset.h"
#include "Curves/CurveFloat.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

void APickupWeapon::BeginPlay()
{
    Super::BeginPlay();

    Mesh->SetGenerateOverlapEvents(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void APickupWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bPickuped)
    {
        AbsorbToTarget();
    }
    else
    {
        FloatingByCurve(DeltaTime);
    }
}

void APickupWeapon::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (WeaponData)
    {
        if (UStaticMesh* StaticMeshData = WeaponData->Mesh.LoadSynchronous())
        {
            Mesh->SetStaticMesh(StaticMeshData);
        }
    }
}

void APickupWeapon::OnPickup(AActor* InTarget)
{
    Super::OnPickup(InTarget);

    bPickuped = true;
    Target = InTarget;
}

void APickupWeapon::FloatingByCurve(float DeltaTime)
{
    ElapsedTime += DeltaTime;

    float MinTime = 0.0f;
    float MaxTime = 0.0f;

    CurveFloat->GetTimeRange(MinTime, MaxTime);

    if (ElapsedTime > MaxTime - MinTime)
    {
        ElapsedTime -= MaxTime - MinTime;
    }

    Mesh->SetRelativeLocation(FVector(0, 0, CurveFloat->GetFloatValue(ElapsedTime) * 60.0f));
    Mesh->AddLocalRotation(FRotator(0.0f, 90.0f, 0.0f) * DeltaTime);
}

void APickupWeapon::AbsorbToTarget()
{
    Mesh->SetWorldLocation(FMath::Lerp(Mesh->GetComponentLocation(), Target->GetActorLocation(), 0.05f));

    if (FVector::Dist(Mesh->GetComponentLocation(), Target->GetActorLocation()) <= 10.0f)
    {
        UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            AbsorbEffectSystem,
            Target->GetActorLocation()
        );
        IWeaponUserInterface::Execute_EquipWeapon(Target, WeaponData);
        Destroy();
    }
}
