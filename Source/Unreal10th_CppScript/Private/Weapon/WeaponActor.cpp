// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponActor.h"
#include "Unreal10th_CppScript/Unreal10th_CppScript.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Interface/WeaponUserInterface.h"
#include "Interface/StatInterface.h"
#include "Interface/HealthInterface.h"
#include "Component/StatComponent.h"

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
        AttachToComponent(OwnerCharacter.Get()->GetMesh(), AttachRule, AttachSocketName);
        HitArea->IgnoreActorWhenMoving(OwnerCharacter.Get(), true); // 자기 자신과 충돌하지 않도록 설정

        IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter);
        WeaponUser->GetWeaponAttackStateChangedDelegate().BindUFunction(this, FName("AttackEnable"));
    }
}

void AWeaponActor::OnHitAreaBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
    UE_LOG(LogTemp, Log, TEXT("오버랩된 대상 : %s"), *InOtherActor->GetName());

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

    IHealthInterface::Execute_DamageHealth(StatComp, WeaponDamage);

    UE_LOG(LogTemp, Log, TEXT("무기로 피해를 입혔습니다 : %f"), WeaponDamage);
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
