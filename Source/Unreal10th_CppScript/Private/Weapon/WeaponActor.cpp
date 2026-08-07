// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponActor.h"

#include "Interface/WeaponUserInterface.h"
#include "Interface/StatInterface.h"
#include "Interface/HealthInterface.h"
#include "Component/StatComponent.h"
#include "Component/WeaponComponent.h"
#include "Data/Item/WeaponDataAsset.h"

#include "Unreal10th_CppScript/Unreal10th_CppScript.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AWeaponActor::AWeaponActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RootMesh"));
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

    TrailVfx = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailVfx"));
    TrailVfx->SetupAttachment(Mesh);
    TrailVfx->bAutoActivate = false;
}

void AWeaponActor::InitializeWeapon(UWeaponDataAsset* InData)
{
    if (!InData)
    {
        return;
    }

    WeaponData = InData;

    // 로딩이 완료되었을 때만 처리
    if (WeaponData->IsLoaded())
    {
        Mesh->SetSkeletalMesh(WeaponData->Mesh.Get());
        TrailVfx->SetAsset(WeaponData->TrailVfx.Get());
    }

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
    if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter))
    {
        if (UWeaponComponent* WeaponComp = WeaponUser->GetWeaponComponent())
        {
            WeaponComp->GetWeaponAttackStateChangedDelegate().Clear();
        }
    }

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
        FTimerDelegate::CreateWeakLambda(
            this,
            [this]() {
                Mesh->SetCollisionResponseToChannel(ECC_Player, ECollisionResponse::ECR_Block);
            }
        ),
        PhysicsDelay,
        false
    );

    // 뒤로 던지기
    FVector BackwardDirection =
        OwnerCharacter.IsValid()
        ? -OwnerCharacter->GetActorForwardVector()
        : FVector::BackwardVector;
    FVector ThrowDirection = BackwardDirection * 500.0f + FVector::UpVector * 400.0f;
    Mesh->AddImpulse(ThrowDirection, NAME_None, true);
    FVector AngularImpulse = FVector(FMath::RandRange(-200.0f, 200.0f)) + GetActorForwardVector() * 1000.0f;
    Mesh->AddAngularImpulseInDegrees(AngularImpulse, NAME_None, true);

    SetLifeSpan(DropLiftSpan);

    //OnWeaponDrop.ExecuteIfBound(WeaponData);
    OnWeaponDrop.Unbind();
    OwnerCharacter = nullptr;
}

void AWeaponActor::Use()
{
    if (WeaponData && !WeaponData->bInfinityUse)
    {
        CurrentUseCount--;
        UE_LOG(LogTemp, Log, TEXT("%s의 남은 무기 사용 횟수 : %d"), *GetName(), CurrentUseCount);

        if (CurrentUseCount <= 0)
        {
            //DropWeapon();
            OnWeaponDrop.ExecuteIfBound(WeaponData);
        }
    }
}

void AWeaponActor::ResetUseCount()
{
    CurrentUseCount = WeaponData->UseCount;
    UE_LOG(LogTemp, Log, TEXT("%s의 남은 무기 사용 횟수 : %d"), *GetName(), CurrentUseCount);
}

FVector AWeaponActor::GetWeaponImpactLocation() const
{
    return FMath::Lerp(Mesh->GetSocketLocation(TEXT("Base")), Mesh->GetSocketLocation(TEXT("Tip")), 0.5f);
}

void AWeaponActor::BeginPlay()
{
    Super::BeginPlay();

    HitArea->OnComponentBeginOverlap.AddDynamic(this, &AWeaponActor::OnHitAreaBeginOverlap);
    TrailVfx->Deactivate();
}

void AWeaponActor::OnHitAreaBeginOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor, UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bFromSweep, const FHitResult& InSweepResult)
{
    if (!OwnerCharacter.IsValid() || !InOtherActor)
    {
        return;
    }

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
    if (bEnable)
    {
        HitArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        TrailVfx->Activate();
        UE_LOG(LogTemp, Log, TEXT("남은 무기 사용 가능 횟수 : %d"), CurrentUseCount);
    }
    else
    {
        HitArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        TrailVfx->Deactivate();
    }
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

        // 자기 자신과 충돌하지 않도록 설정
        HitArea->IgnoreActorWhenMoving(OwnerCharacter.Get(), true);

        if (IWeaponUserInterface* WeaponUser = Cast<IWeaponUserInterface>(OwnerCharacter))
        {
            if (UWeaponComponent* WeaponComp = WeaponUser->GetWeaponComponent())
            {
                WeaponComp->GetWeaponAttackStateChangedDelegate().BindUFunction(this, FName("AttackEnable"));
            }
        }
    }
}
