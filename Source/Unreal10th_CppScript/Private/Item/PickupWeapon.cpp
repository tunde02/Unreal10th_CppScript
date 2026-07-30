// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupWeapon.h"
#include "Weapon/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"
#include "Data/WeaponDataAsset.h"
#include "Curves/CurveFloat.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"

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

    //if (bPickuped)
    //{
    //    AbsorbToTarget();
    //}
    //else
    //{
    //    FloatingByCurve(DeltaTime);
    //}
}

void APickupWeapon::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (WeaponData)
    {
        if (UStaticMesh* StaticMeshData = WeaponData->Mesh.LoadSynchronous())
        {
            Mesh->SetStaticMesh(StaticMeshData);
            Mesh->SetRelativeLocation(MeshBaseLocation + WeaponData->LocationOffset);
        }
    }
}

void APickupWeapon::OnPickup(AActor* InTarget)
{
    /* DEPRECATED
    bPickuped = true;
    Target = InTarget;
    */

    Super::OnPickup(InTarget);

    TargetActor = InTarget;

    // 에셋이 준비되어 있지 않으면 즉시 획득 처리
    if (!IsPickupEffectAssetReady())
    {
        OnFinishPickupEffect();
        return;
    }

    // 더 이상 오버랩이 발생하지 않게 하기
    SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PickupElapsedTime = 0.0f;
    PickupStartLocation = Mesh->GetComponentLocation();

    GetWorldTimerManager().SetTimer(
        PickupEffectTimerHandle,
        this,
        &APickupWeapon::OnUpdatePickupEffect,
        TimerInterval,
        true
    );
}

void APickupWeapon::OnUpdatePickupEffect()
{
    if (!TargetActor.IsValid())
    {
        OnFinishPickupEffect();
        return;
    }

    PickupElapsedTime += TimerInterval;
    float Progress = PickupElapsedTime / PickupEffectDuration;

    float DistanceAlpha = PickupAlpha->GetFloatValue(Progress);
    FVector Goal = TargetActor.Get()->GetActorLocation();
    FVector NewLocation = FMath::Lerp(PickupStartLocation, Goal, DistanceAlpha);

    float HeightOffset = PickupHeight->GetFloatValue(Progress) * PickupEffectHeight;
    NewLocation.Z += HeightOffset;
    Mesh->SetWorldLocation(NewLocation);

    float Scale = PickupScale->GetFloatValue(Progress);
    Mesh->SetRelativeScale3D(FVector(Scale));

    if (Progress >= 1.0f)
    {
        OnFinishPickupEffect();
    }
}

void APickupWeapon::OnFinishPickupEffect()
{
    if (TargetActor.IsValid())
    {
        UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            AbsorbEffectSystem,
            TargetActor->GetActorLocation()
        );
        IWeaponUserInterface::Execute_EquipWeapon(TargetActor.Get(), WeaponData);
    }

    GetWorldTimerManager().ClearTimer(PickupEffectTimerHandle);
    Destroy();
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

bool APickupWeapon::IsPickupEffectAssetReady() const
{
    return PickupAlpha != nullptr && PickupHeight != nullptr && PickupScale != nullptr;
}
