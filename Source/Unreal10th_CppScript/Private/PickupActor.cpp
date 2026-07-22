// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Interface/StaminaInterface.h"

// Sets default values
APickupActor::APickupActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RootCollision"));
    SphereCollision->InitSphereRadius(100.0f);
    SetRootComponent(SphereCollision);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void APickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
    // Target이 null이 아니면 인터페이스를 상속받았다
    // 블루프린트에서 상속을 했을 경우는 체크 불가능
    //IStaminaInterface* Target = Cast<IStaminaInterface>(OtherActor);

    // bImplements이 true면 인터페이스를 구현했다
    //bool bImplements = OtherActor->Implements<UStaminaInterface>();

    if (OtherActor && OtherActor->Implements<UStaminaInterface>())
    {
        if (Stamina > 0)
        {
            IStaminaInterface::Execute_RecoveryStamina(OtherActor, Stamina);
        }
        else
        {
            IStaminaInterface::Execute_ConsumeStamina(OtherActor, -Stamina);
        }
    }
}

