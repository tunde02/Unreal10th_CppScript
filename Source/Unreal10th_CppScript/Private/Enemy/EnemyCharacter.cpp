// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Component/StatComponent.h"
#include "Unreal10th_CppScript/Unreal10th_CppScript.h"
#include "Interface/HealthInterface.h"
#include "CommonHeader/ItemDropTable.h"
#include "Item/PickupBase.h"
#include "Enemy/DamagePopupActor.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"
#include "Framework/Subsystem/PickupFactorySubsystem.h"

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
    if (!GetWorld())
    {
        return;
    }

    if (!ItemDropTable)
    {
        return;
    }

    /* GetAllRows()를 사용하는 방식
    TArray<FItemDropTableRow*> AllRows;
    ItemDropTable->GetAllRows(TEXT("AEnemyCharacter::DropItems"), AllRows);
    for (FItemDropTableRow* Row : AllRows) {}
    */

    TMap<FName, uint8*> Map = ItemDropTable->GetRowMap();
    for (const auto& [RowName, Value] : Map)
    {
        FItemDropTableRow* Row = reinterpret_cast<FItemDropTableRow*>(Value);

        // 필수 데이터 확인
        if (!Row || !Row->PickupData || !Row->PickupData->PickupClass)
        {
            continue;
        }

        // 확률 체크
        float RandomFloat = FMath::FRand();
        if (RandomFloat > Row->DropRate)
        {
            continue;
        }

        UPickupFactorySubsystem* PickupFactory = GetWorld()->GetSubsystem<UPickupFactorySubsystem>();
        if (PickupFactory)
        {
            PickupFactory->SpawnPickupAsync(
                Row->PickupData,
                GetActorTransform(),
                FOnPickupSpawned::CreateWeakLambda(
                    this,
                    [](APickupBase* InSpawned) {
                        UE_LOG(LogTemp, Log, TEXT("%s가 스폰되었습니다."), *InSpawned->GetName());
                    }
                )
            );
        }
    }
}

void AEnemyCharacter::HandleOnDie(UAnimMontage* InMontage, bool bInterrupted)
{
    DropItems();

    Destroy();
}
