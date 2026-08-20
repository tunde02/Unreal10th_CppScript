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
    SphereCollision->SetCollisionResponseToChannel(ECC_Player, ECR_Overlap);
    SetRootComponent(SphereCollision);

    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
    NiagaraComponent->SetupAttachment(SphereCollision);
}

void APickupBase::InitializePickup(UItemDataAsset* InData)
{
    DataAsset = InData;
}

void APickupBase::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    InitializePickup(DataAsset);
}

void APickupBase::BeginPlay()
{
    Super::BeginPlay();

    ElapsedTime = 0.0f;

    NiagaraComponent->SetRelativeLocation(MeshZOffset);

    FTimerHandle PickupDelayHandle;
    GetWorld()->GetTimerManager().SetTimer(
        PickupDelayHandle,
        [this]() {
            OnActorBeginOverlap.AddDynamic(this, &APickupBase::OnBeginOverlap);
        },
        PickupDelayTime,
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

void APickupBase::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    OnPickup(OtherActor);
}

void APickupBase::OnPickup(AActor* InTarget)
{
    // 타이머가 이미 작동중이면 종료(중복실행 방지)
    if (GetWorldTimerManager().IsTimerActive(PickupEffectTimerHandle))
    {
        return;
    }

    //UE_LOG(LogTemp, Log, TEXT("%s(이)가 %s를 획득했습니다."),
    //       InTarget ? *InTarget->GetName() : TEXT("알 수 없는 대상"), *GetName());
    bIdle = false;
    TargetActor = InTarget;

    // 커브 에셋이 준비되어 있고 메시 컴포넌트가 있으면 연출 시작, 없으면 즉시 획득 처리
    if (IsPickupEffectAssetReady() && GetMesh())
    {
        // 더 이상의 오버랩이 발생하지 않게 하기
        SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        PickupStartLocation = GetMesh()->GetComponentLocation();
        PickupElapsedTime = 0.0f;

        GetWorldTimerManager().SetTimer(
            PickupEffectTimerHandle,
            this,
            &APickupBase::OnUpdatePickupEffect,
            TimerInterval,
            true
        );
    }
    else
    {
        OnFinishPickupEffect();
    }
}

void APickupBase::OnUpdatePickupEffect()
{
    if (!TargetActor.IsValid() || !GetMesh())
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
    // 획득 이팩트용 타이머 클리어
    GetWorldTimerManager().ClearTimer(PickupEffectTimerHandle);

    // 대상의 인벤토리에 아이템 추가
    if (IInventoryUserInterface* InventoryUser = Cast<IInventoryUserInterface>(TargetActor))
    {
        FInventoryCommand Command = FInventoryCommand::MakeAddCommand(DataAsset, 1);
        FInventoryCommandResult Result;
        if (!InventoryUser->ExecuteInventoryCommand(Command, Result))
        {
            // 실패하면 다시 스폰
            UPickupFactorySubsystem* Factory = GetWorld()->GetSubsystem<UPickupFactorySubsystem>();
            FTransform SpawnTransform = TargetActor->GetActorTransform();
            FVector NewLocation(FMath::RandPointInCircle(300.0f), 0.0f);	// 액터 위치를 중심으로 반경 3m의 서클 안 랜덤 위치
            SpawnTransform.AddToTranslation(NewLocation);
            Factory->SpawnPickupAsync(
                DataAsset,
                SpawnTransform,
                FOnPickupSpawned::CreateWeakLambda(
                    this,
                    [this](APickupBase* InSpawned) {
                        UE_LOG(LogTemp, Log, TEXT("%s가 스폰되었습니다."), *InSpawned->GetName());
                        Destroy(); // 기존에 먹었던 픽업은 삭제
                    }
                ));
        }
        else
        {
            // 성공했으면 인벤토리에 들어갔으니 픽업 삭제
            Destroy();
        }
    }
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
