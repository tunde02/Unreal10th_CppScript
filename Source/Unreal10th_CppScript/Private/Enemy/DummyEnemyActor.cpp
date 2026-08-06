// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DummyEnemyActor.h"
#include "Enemy/DamagePopupActor.h"
#include "Test/TestVfxPlayer.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"

// Sets default values
ADummyEnemyActor::ADummyEnemyActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    PopupTransform = CreateDefaultSubobject<USceneComponent>(TEXT("PopupTransform"));
    PopupTransform->SetupAttachment(Mesh);
    PopupTransform->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void ADummyEnemyActor::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void ADummyEnemyActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

float ADummyEnemyActor::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UObjectPoolSubsystem* Subsystem = GameInstance->GetSubsystem<UObjectPoolSubsystem>();
        ADamagePopupActor* DamagePopup = Subsystem->Spawn<ADamagePopupActor>(DamagePopupClass, PopupTransform->GetComponentTransform());

        if (DamagePopup)
        {
            DamagePopup->OnPopupStart(Damage);
        }

        ATestVfxPlayer* TestVfxPlayer = Subsystem->Spawn<ATestVfxPlayer>(TextVfxPlayerClass, PopupTransform->GetComponentTransform());
    }

    return Damage;
}

