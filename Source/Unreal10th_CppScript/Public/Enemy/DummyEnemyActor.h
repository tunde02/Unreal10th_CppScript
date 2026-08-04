// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DummyEnemyActor.generated.h"

class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API ADummyEnemyActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ADummyEnemyActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USceneComponent> PopupTransform = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hello")
    TSubclassOf<AActor> DamagePopupClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Hello")
    TSubclassOf<AActor> TextVfxPlayerClass = nullptr;

};
