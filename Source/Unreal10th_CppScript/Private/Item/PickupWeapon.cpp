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

void APickupWeapon::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    InitializePickup(DataAsset);
}

//void APickupWeapon::OnPickup(AActor* InTarget)
//{
//    Super::OnPickup(InTarget);
//
//    TargetActor = InTarget;
//
//    // 에셋이 준비되어 있지 않으면 즉시 획득 처리
//    if (!IsPickupEffectAssetReady())
//    {
//        OnFinishPickupEffect();
//        return;
//    }
//
//    // 더 이상 오버랩이 발생하지 않게 하기
//    SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//
//    PickupElapsedTime = 0.0f;
//    PickupStartLocation = Mesh->GetComponentLocation();
//
//    GetWorldTimerManager().SetTimer(
//        PickupEffectTimerHandle,
//        this,
//        &APickupWeapon::OnUpdatePickupEffect,
//        TimerInterval,
//        true
//    );
//}
//
//void APickupWeapon::OnUpdatePickupEffect()
//{
//    if (!TargetActor.IsValid())
//    {
//        OnFinishPickupEffect();
//        return;
//    }
//
//    PickupElapsedTime += TimerInterval;
//
//    float Duration = FMath::Max(PickupEffectDuration, 0.001f);
//    float Progress = PickupElapsedTime / Duration;
//
//    float DistanceAlpha = PickupAlpha->GetFloatValue(Progress);
//    FVector Goal = TargetActor.Get()->GetActorLocation();
//    FVector NewLocation = FMath::Lerp(PickupStartLocation, Goal, DistanceAlpha);
//
//    float HeightOffset = PickupHeight->GetFloatValue(Progress) * PickupEffectHeight;
//    NewLocation.Z += HeightOffset;
//    Mesh->SetWorldLocation(NewLocation);
//
//    float Scale = PickupScale->GetFloatValue(Progress);
//    Mesh->SetRelativeScale3D(FVector(Scale));
//
//    if (Progress >= 1.0f)
//    {
//        OnFinishPickupEffect();
//    }
//}

void APickupWeapon::OnFinishPickupEffect()
{
    if (TargetActor.IsValid() && WeaponData.IsValid())
    {
        UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            PickupVfx,
            TargetActor->GetActorLocation()
        );

        if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(TargetActor.Get()))
        {
            IWeaponUserInterface::Execute_EquipWeapon(TargetActor.Get(), WeaponData.Get());
        }
    }

    GetWorldTimerManager().ClearTimer(PickupEffectTimerHandle);

    //Destroy();
    Super::OnFinishPickupEffect();
}

UMeshComponent* APickupWeapon::GetMesh() const
{
    return Mesh;
}

//bool APickupWeapon::IsPickupEffectAssetReady() const
//{
//    return PickupAlpha != nullptr && PickupHeight != nullptr && PickupScale != nullptr;
//}
