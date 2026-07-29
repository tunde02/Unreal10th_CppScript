// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponActor.h"
#include "Unreal10th_CppScript/Unreal10th_CppScript.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Interface/WeaponUserInterface.h"
#include "Interface/StatInterface.h"
#include "Interface/HealthInterface.h"
#include "Component/StatComponent.h"
#include "Data/WeaponDataAsset.h"

// Sets default values
AWeaponActor::AWeaponActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootMesh"));
    SetRootComponent(Mesh);
    Mesh->SetGenerateOverlapEvents(false);
    Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

    HitArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitArea"));
    HitArea->SetupAttachment(Mesh);
    HitArea->SetCapsuleHalfHeight(60.0f, false);
    HitArea->SetCapsuleRadius(30.0f, false);
    HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HitArea->SetCollisionObjectType(ECC_Weapon);
    HitArea->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    HitArea->SetCollisionResponseToChannel(ECC_Enemy, ECollisionResponse::ECR_Overlap);
    HitArea->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
}

void AWeaponActor::InitializeWeapon(UWeaponDataAsset* InData)
{
    WeaponData = InData;
    Mesh->SetStaticMesh(WeaponData->Mesh.Get());
    Mesh->SetRelativeLocation(WeaponData->LocationOffset);
    HitArea->SetCapsuleHalfHeight(WeaponData->HitAreaHalfHeight, false);
    HitArea->SetCapsuleRadius(WeaponData->HitAreaRadius, false);
}

void AWeaponActor::EquipToTarget(AActor* InTarget)
{
    OnEquipped(InTarget);
}

void AWeaponActor::DropWeapon()
{
    FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
    DetachFromActor(DetachRules);

    Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
    Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);
    Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    Mesh->SetSimulatePhysics(true);
    HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    FVector RandomDirection = FMath::VRandCone(FVector::UpVector, FMath::DegreesToRadians(45.0f));
    float RandomForce = FMath::FRandRange(700.0f, 1200.0f);
    Mesh->AddImpulse(RandomDirection * RandomForce, NAME_None, true);
    Mesh->AddAngularImpulseInDegrees(FVector(FMath::FRandRange(0.0f, 1.0f), FMath::FRandRange(0.0f, 1.0f), FMath::FRandRange(0.0f, 1.0f)) * 500.0f, NAME_None, true);
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
    Super::BeginPlay();

    HitArea->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnHitAreaBeginOverlap);
}

void AWeaponActor::OnEquipped(AActor* InOwner)
{
    SetOwner(InOwner);
    OwnerCharacter = Cast<ACharacter>(InOwner);
    FAttachmentTransformRules AttachRule(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        true
    );

    if (OwnerCharacter.IsValid())
    {
        AttachToComponent(OwnerCharacter.Get()->GetMesh(), AttachRule, WeaponData->AttachSocketName);
        HitArea->IgnoreActorWhenMoving(OwnerCharacter.Get(), true); // 자기 자신과 충돌하지 않도록 설정

        IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter);
        if (WeaponUser)
        {
            WeaponUser->GetWeaponAttackStateChangedDelegate().BindUFunction(this, FName("AttackEnable"));
        }
    }
}

void AWeaponActor::OnHitAreaBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("오버랩된 대상 : %s"), *InOtherActor->GetName());

    /*
    if (!InOtherActor->Implements<UStatInterface>())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s 이 StatInterface를 구현하고 있지 않습니다!!"), *InOtherActor->GetName());
        return;
    }

    UStatComponent* StatComp = Cast<IStatInterface>(InOtherActor)->GetStatComponent();

    if (!StatComp->Implements<UHealthInterface>())
    {
        UE_LOG(LogTemp, Warning, TEXT("%s 의 StatComponent가 HealthInterface를 구현하고 있지 않습니다!!"), *InOtherActor->GetName());
        return;
    }

    IHealthInterface::Execute_DamageHealth(StatComp, AttackPower);

    UE_LOG(LogTemp, Log, TEXT("무기로 피해를 입혔습니다 : %f"), AttackPower);
    */

    UGameplayStatics::ApplyDamage(InOtherActor, WeaponData->AttackPower, OwnerCharacter->GetController(), this, nullptr);
}

void AWeaponActor::AttackEnable(bool bEnable)
{
    if (bEnable)
    {
        HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
    else
    {
        HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
