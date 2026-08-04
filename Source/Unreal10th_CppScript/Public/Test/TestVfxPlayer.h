// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PoolableInterface.h"
#include "TestVfxPlayer.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API ATestVfxPlayer : public AActor, public IPoolableInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ATestVfxPlayer();

    UFUNCTION(BlueprintCallable)
    virtual void UseFinish() override;

    virtual void OnSpawn_Implementation() override;

    virtual void OnReturn_Implementation() override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION()
    void OnVfxFinished(UNiagaraComponent* InNiagaraComponent);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UNiagaraComponent> Vfx = nullptr;

};
