// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/TestVfxPlayer.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"

// Sets default values
ATestVfxPlayer::ATestVfxPlayer()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    Vfx = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Vfx"));
    Vfx->SetupAttachment(Mesh);
}

void ATestVfxPlayer::UseFinish()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UObjectPoolSubsystem* Subsystem = GameInstance->GetSubsystem<UObjectPoolSubsystem>();
        Subsystem->ReturnPool(this->GetClass(), this);
    }
}

void ATestVfxPlayer::OnSpawn_Implementation()
{
    SetActorHiddenInGame(false);
    Vfx->Activate();

}

void ATestVfxPlayer::OnReturn_Implementation()
{
    SetActorLocation(FVector(0.0f, 0.0f, -10000.0f));
    SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ATestVfxPlayer::BeginPlay()
{
    Super::BeginPlay();

    Vfx->OnSystemFinished.AddDynamic(this, &ATestVfxPlayer::OnVfxFinished);
}

// Called every frame
void ATestVfxPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ATestVfxPlayer::OnVfxFinished(UNiagaraComponent* InNiagaraComponent)
{
    UseFinish();
}

