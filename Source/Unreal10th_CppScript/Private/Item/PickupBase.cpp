// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupBase.h"
#include "Unreal10th_CppScript/Unreal10th_CppScript.h"
#include "Data/Item/ItemDataAsset.h"

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

void APickupBase::BeginPlay()
{
    Super::BeginPlay();

    ElapsedTime = 0.0f;

    NiagaraComponent->SetRelativeLocation(MeshZOffset);
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
    UE_LOG(LogTemp, Log, TEXT("%s(이)가 %s를 획득했습니다."),
           InTarget ? *InTarget->GetName() : TEXT("알 수 없는 대상"),
           *GetName());
    bIdle = false;
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

