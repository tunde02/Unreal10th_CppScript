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
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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
    if (!InData)
    {
        return;
    }

    WeaponData = InData;
    Mesh->SetStaticMesh(WeaponData->Mesh.Get());
    HitArea->SetCapsuleHalfHeight(WeaponData->HitAreaHalfHeight, false);
    HitArea->SetCapsuleRadius(WeaponData->HitAreaRadius, false);

    CurrentUseCount = WeaponData->UseCount;
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
    Mesh->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Ignore);
    Mesh->SetSimulatePhysics(true);
    HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 일정 시간 이후에 무기와 플레이어가 다시 충돌 가능하도록 설정
    FTimerManager& TimerManager = GetWorld()->GetTimerManager();
    TimerManager.SetTimer(
        PhysicsDelayTimerHandle,
        FTimerDelegate::CreateLambda(
            [this]() {
                Mesh->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);
            }
        ),
        PhysicsDelay,
        false
    );

    // 뒤로 던지기
    FVector BackwardDirection = -OwnerCharacter->GetActorForwardVector();
    FVector ThrowDirection = BackwardDirection * 500.0f + FVector::UpVector * 400.0f;
    Mesh->AddImpulse(ThrowDirection, NAME_None, true);
    FVector AngularImpulse = FVector(FMath::RandRange(-200.0f, 200.0f)) + GetActorForwardVector() * 1000.0f;
    Mesh->AddAngularImpulseInDegrees(AngularImpulse, NAME_None, true);

    SetLifeSpan(DropLiftSpan);

    OwnerCharacter = nullptr;
}

// Called when the game starts or when spawned
void AWeaponActor::BeginPlay()
{
    Super::BeginPlay();

    HitArea->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnHitAreaBeginOverlap);
}

void AWeaponActor::OnEquipped(AActor* InOwner)
{
    if (!WeaponData)
    {
        return;
    }

    SetOwner(InOwner);
    OwnerCharacter = Cast<ACharacter>(InOwner);
    FAttachmentTransformRules AttachRules(
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        EAttachmentRule::SnapToTarget,
        true
    );

    if (OwnerCharacter.IsValid())
    {
        AttachToComponent(OwnerCharacter.Get()->GetMesh(), AttachRules, WeaponData->AttachSocketName);

        // Offset 적용
        SetActorRelativeLocation(WeaponData->LocationOffset);

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
    float Damage = WeaponData ? WeaponData->AttackPower : 1.0f;

    UE_LOG(LogTemp, Log, TEXT("오버랩된 대상 : %s"), *InOtherActor->GetName());

    UGameplayStatics::ApplyDamage(InOtherActor, Damage, OwnerCharacter->GetController(), this, nullptr);

    UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
        GetWorld(),
        WeaponData->WeaponHitVfx.Get(),
        GetActorLocation()
    );
}

void AWeaponActor::AttackEnable(bool bEnable)
{
    if (CurrentUseCount < 1)
    {
        IWeaponUserInterface::Execute_EquipBasicWeapon(OwnerCharacter.Get());
        return;
    }

    if (bEnable)
    {
        HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        CurrentUseCount--;
        UE_LOG(LogTemp, Log, TEXT("남은 무기 사용 가능 횟수 : %d"), CurrentUseCount);
    }
    else
    {
        HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
