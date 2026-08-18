// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupBase.h"
#include "Unreal10th_CppScript/Unreal10th_CppScript.h"
#include "Data/Item/ItemDataAsset.h"
#include "Interface/InventoryUserInterface.h"
#include "Component/InventoryCommandTypes.h"
#include "Framework/Subsystem/PickupFactorySubsystem.h"

#include "Components/SphereComponent.h"
#include "Components/MeshComponent.h"
#include "NiagaraComponent.h"

APickupBase::APickupBase()
{
    PrimaryActorTick.bCanEverTick = true;

    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
    SphereCollision->InitSphereRadius(100.0f);
    SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    //SphereCollision->SetCollisionResponseToChannel(ECC_Player, ECR_Overlap);
    SetRootComponent(SphereCollision);

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
    NiagaraComponent->SetupAttachment(SphereCollision);
}

void APickupBase::InitializePickup(UItemDataAsset* InData)
{
    DataAsset = InData;
}

void APickupBase::BeginPlay()
{
    Super::BeginPlay();

    ElapsedTime = 0.0f;

    NiagaraComponent->SetRelativeLocation(MeshZOffset);

    FTimerHandle Handle;
    GetWorldTimerManager().SetTimer(
        Handle,
        FTimerDelegate::CreateWeakLambda(
            this,
            [this]() {
                SphereCollision->SetCollisionResponseToChannel(ECC_Player, ECR_Overlap);
            }
        ),
        4.0f,
        false
    );
}

void APickupBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIdle)
    {
        OnUpdateUpDownSpin(DeltaTime);
    }
}

void APickupBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);

    OnPickup(OtherActor);
}

void APickupBase::OnPickup(AActor* InTarget)
{
    if (GetWorldTimerManager().IsTimerActive(PickupEffectTimerHandle))
    {
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("%s(이)가 %s를 획득했습니다."),
           InTarget ? *InTarget->GetName() : TEXT("알 수 없는 대상"), *GetName());
    bIdle = false;
    TargetActor = InTarget;

    if (IInventoryUserInterface* InventoryUser = Cast<IInventoryUserInterface>(TargetActor))
    {
        FInventoryCommand Command = FInventoryCommand::MakeAddCommand(DataAsset, 1);
        FInventoryCommandResult Result;
        if (!InventoryUser->ExecuteInventoryCommand(Command, Result))
        {
            UPickupFactorySubsystem* Factory = GetWorld()->GetSubsystem<UPickupFactorySubsystem>();
            Factory->SpawnPickupAsync(
                DataAsset,
                TargetActor->GetActorTransform(),
                FOnPickupSpawned::CreateWeakLambda(
                    this,
                    [this](APickupBase* InSpawned) {
                        HandlePickupEffect();
                    }
                )
            );
        }
        else
        {
            HandlePickupEffect();
        }
    }
}

void APickupBase::OnUpdatePickupEffect()
{
    if (!TargetActor.IsValid())
    {
        OnFinishPickupEffect();
        return;
    }

    PickupElapsedTime += TimerInterval;

    float Duration = FMath::Max(PickupEffectDuration, 0.001f);
    float Progress = PickupElapsedTime / Duration;

    float DistanceAlpha = PickupAlpha->GetFloatValue(Progress);
    FVector Goal = TargetActor.Get()->GetActorLocation();
    FVector NewLocation = FMath::Lerp(PickupStartLocation, Goal, DistanceAlpha);

    float HeightOffset = PickupHeight->GetFloatValue(Progress) * PickupEffectHeight;
    NewLocation.Z += HeightOffset;
    GetMesh()->SetWorldLocation(NewLocation);

    float Scale = PickupScale->GetFloatValue(Progress);
    GetMesh()->SetRelativeScale3D(FVector(Scale));

    if (Progress >= 1.0f)
    {
        OnFinishPickupEffect();
    }
}

void APickupBase::OnFinishPickupEffect()
{
    Destroy();
}

void APickupBase::OnUpdateUpDownSpin(float InDeltaTime)
{
    if (!IsCurveAssetReady())
    {
        return;
    }

    ElapsedTime += InDeltaTime;

    if (UMeshComponent* PickupMesh = GetMesh())
    {
        float Duration = FMath::Max(UpDownDuration, 0.001f);
        float Progress = FMath::Fmod(ElapsedTime / Duration, 1.0f);
        FVector NewMeshLocation = MeshBaseLocation + DataAsset->SpawnLocationOffset;
        NewMeshLocation.Z += UpDownCurve->GetFloatValue(Progress) * UpDownHeight;

        PickupMesh->SetRelativeLocation(NewMeshLocation);

        float NewAngle = SpinCurve->GetFloatValue(Progress) * 360.0f;

        PickupMesh->SetRelativeRotation(FRotator(0.0f, NewAngle, 0.0f));
    }
}

UMeshComponent* APickupBase::GetMesh() const
{
    return nullptr;
}

bool APickupBase::IsCurveAssetReady() const
{
    return UpDownCurve != nullptr && SpinCurve != nullptr;
}

bool APickupBase::IsPickupEffectAssetReady() const
{
    return PickupAlpha != nullptr && PickupHeight != nullptr && PickupScale != nullptr;
}

void APickupBase::HandlePickupEffect()
{
    // 에셋이 준비되어 있지 않으면 즉시 획득 처리
    if (!IsPickupEffectAssetReady())
    {
        UE_LOG(LogTemp, Log, TEXT("에셋이 준비되어 있지 않아 즉시 획득"));
        OnFinishPickupEffect();
        return;
    }

    // 더 이상 오버랩이 발생하지 않게 하기
    SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PickupElapsedTime = 0.0f;
    PickupStartLocation = GetMesh()->GetComponentLocation();

    GetWorldTimerManager().SetTimer(
        PickupEffectTimerHandle,
        this,
        &APickupBase::OnUpdatePickupEffect,
        TimerInterval,
        true
    );
}
