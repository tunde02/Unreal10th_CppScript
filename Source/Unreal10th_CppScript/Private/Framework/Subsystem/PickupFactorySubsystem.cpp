// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Subsystem/PickupFactorySubsystem.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Data/Item/ItemDataAsset.h"
#include "Item/PickupBase.h"

void UPickupFactorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

void UPickupFactorySubsystem::SpawnPickup(UItemDataAsset* InItemDataAsset, const FTransform& InTransform)
{
    if (!InItemDataAsset->IsLoaded())
    {
        InItemDataAsset->RequestDataLoad(
            FStreamableDelegate::CreateWeakLambda(
                this,
                [this, InItemDataAsset, InTransform]() {
                    InternalSpawnPickup(InItemDataAsset, InTransform);
                }
            )
        );
    }
    else
    {
        InternalSpawnPickup(InItemDataAsset, InTransform);
    }
}

void UPickupFactorySubsystem::InternalSpawnPickup(UItemDataAsset* InItemDataAsset, const FTransform& InTransform)
{
    APickupBase* PickupActor = GetWorld()->SpawnActor<APickupBase>(
        InItemDataAsset->PickupClass.Get(),
        InTransform
    );

    if (!PickupActor)
    {
        return;
    }

    PickupActor->InitializePickup(InItemDataAsset);
    HandlePickupItemBounce(PickupActor);

    UE_LOG(LogTemp, Log, TEXT("[PickupFactory] %s가 드랍되었습니다."), *(InItemDataAsset->DisplayName).ToString());
}

void UPickupFactorySubsystem::HandlePickupItemBounce(AActor* InActor) const
{
    UPrimitiveComponent* PhysComp = Cast<UPrimitiveComponent>(InActor->GetRootComponent());

    if (!PhysComp)
    {
        PhysComp = InActor->FindComponentByClass<UPrimitiveComponent>();
    }

    if (!PhysComp)
    {
        return;
    }

    PhysComp->SetSimulatePhysics(true);

    FVector RandomDir = FVector(
        FMath::FRandRange(-1.0f, 1.0f),
        FMath::FRandRange(-1.0f, 1.0f),
        FMath::FRandRange(1.0f, 1.0f) // 위로 튀도록
    ).GetSafeNormal();

    float ImpulseStrength = 400.0f;
    PhysComp->AddImpulse(RandomDir * ImpulseStrength, NAME_None, true);

    // 일정 시간 후 물리를 끄는 람다(Lambda) 타이머 설정
    float PhysicsDuration = 3.0f;
    FTimerHandle TimerHandle;

    // TWeakObjectPtr를 써서 타이머 동작 중에 액터가 파괴되어 발생하는 널 참조 예방
    TWeakObjectPtr<UPrimitiveComponent> WeakPhysComp = PhysComp;

    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        [WeakPhysComp]() {
            if (WeakPhysComp.IsValid())
            {
                WeakPhysComp->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
                WeakPhysComp->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
                WeakPhysComp->SetSimulatePhysics(false);
            }
        },
        PhysicsDuration,
        false
    );
}
