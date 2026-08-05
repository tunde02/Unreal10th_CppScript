// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/DamagePopupActor.h"
#include "Widget/DamagePopupWidget.h"
#include "Framework/Subsystem/ObjectPoolSubsystem.h"

#include "Components/WidgetComponent.h"

// Sets default values
ADamagePopupActor::ADamagePopupActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    DamagePopupWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamagePopupWidgetComponent"));
    SetRootComponent(DamagePopupWidgetComponent);

    DamagePopupWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    //DamagePopupWidgetComponent->SetCastShadow(false); // 월드스페이스일 때 그림자 발생 방지
}

void ADamagePopupActor::OnPopupStart(float InDamage)
{
    if (DamagePopupWidget)
    {
        DamagePopupWidget->SetDamageText(InDamage);
        DamagePopupWidget->PlayPopupAnimation();
    }
}

void ADamagePopupActor::UseFinish()
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        UObjectPoolSubsystem* Subsystem = GameInstance->GetSubsystem<UObjectPoolSubsystem>();
        Subsystem->ReturnPool(this);
    }
}

void ADamagePopupActor::OnSpawn_Implementation()
{
    SetActorHiddenInGame(false);
    //OnPopupStart(111.1f);
}

void ADamagePopupActor::OnReturn_Implementation()
{
    SetActorLocation(FVector(0.0f, 0.0f, -10000.0f));
    SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ADamagePopupActor::BeginPlay()
{
    Super::BeginPlay();

    DamagePopupWidget = Cast<UDamagePopupWidget>(DamagePopupWidgetComponent->GetWidget());
    DamagePopupWidget->OnPopupAnimationFinished.BindUObject(this, &ADamagePopupActor::UseFinish);
}

// Called every frame
void ADamagePopupActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

