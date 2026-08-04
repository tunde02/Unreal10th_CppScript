// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/PoolableInterface.h"
#include "DamagePopupActor.generated.h"

class UWidgetComponent;
class UDamagePopupWidget;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API ADamagePopupActor : public AActor, public IPoolableInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ADamagePopupActor();

    UFUNCTION(BlueprintCallable)
    void OnPopupStart(float InDamage);

    UFUNCTION(BlueprintCallable)
    virtual void UseFinish() override;

    virtual void OnSpawn_Implementation() override;

    virtual void OnReturn_Implementation() override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadonly)
    TObjectPtr<UWidgetComponent> DamagePopupWidgetComponent;

private:
    TObjectPtr<UDamagePopupWidget> DamagePopupWidget = nullptr;

};
