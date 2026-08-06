// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Component/StatComponent.h"
#include "Unreal10th_CppScript/Unreal10th_CppScript.h"
#include "Interface/HealthInterface.h"
#include "CommonHeader/ItemDropTable.h"
#include "Item/PickupWeapon.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Enemy/DamagePopupActor.h"

#include "Components/CapsuleComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    StatComp = CreateDefaultSubobject<UStatComponent>(TEXT("StatComp"));

    GetCapsuleComponent()->SetCollisionObjectType(ECC_Enemy);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UStatComponent* AEnemyCharacter::GetStatComponent() const
{
    return StatComp;
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (IsValid(StatComp))
    {
        StatComp->OnDie.AddDynamic(this, &AEnemyCharacter::OnDie);
    }
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    IHealthInterface::Execute_DamageHealth(StatComp, Damage);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UObjectPoolSubsystem* Subsystem = GameInstance->GetSubsystem<UObjectPoolSubsystem>();
        FTransform PopupTransform = GetActorTransform();
        PopupTransform.SetLocation(GetActorLocation() + FVector(0.0f, 0.0f, 80.0f));
        ADamagePopupActor* DamagePopup = Subsystem->Spawn<ADamagePopupActor>(DamagePopupClass, PopupTransform);

        if (DamagePopup)
        {
            DamagePopup->OnPopupStart(Damage);
        }
    }

    return Damage;
}

void AEnemyCharacter::OnDie()
{
    UE_LOG(LogTemp, Log, TEXT("%s가 죽었습니다."), *this->GetName());

    PlayAnimMontage(DieMontage);

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(this, &AEnemyCharacter::HandleOnDie);
    GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, DieMontage);
}

void AEnemyCharacter::DropItems()
{
    for (const auto& [RowName, Value] : ItemDropTable->GetRowMap())
    {
        FItemDropTableRow* Row = reinterpret_cast<FItemDropTableRow*>(Value);

        if (!Row)
        {
            continue;
        }

        float RandomFloat = FMath::RandRange(0.0f, 1.0f);

        if (RandomFloat <= Row->DropRate)
        {
            APickupWeapon* SpawnedPickupWeapon = GetWorld()->SpawnActor<APickupWeapon>(
                PickupWeaponClass,
                this->GetActorTransform()
            );

            if (!SpawnedPickupWeapon)
            {
                continue;
            }

            SpawnedPickupWeapon->SetWeaponData(Row->DropItemClass);
            HandlePickupItemBounce(SpawnedPickupWeapon);

            UE_LOG(LogTemp, Log, TEXT("%s 드랍 (DropRate=%.2f, RandomFloat=%.2f)"), *RowName.ToString(), Row->DropRate, RandomFloat);
        }
    }
}

void AEnemyCharacter::HandleOnDie(UAnimMontage* InMontage, bool bInterrupted)
{
    DropItems();

    Destroy();
}

void AEnemyCharacter::HandlePickupItemBounce(AActor* InActor) const
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

