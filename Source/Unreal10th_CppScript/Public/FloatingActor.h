// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingActor.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AFloatingActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AFloatingActor();

    UFUNCTION(BlueprintCallable)
    void FloatingByCos(UStaticMeshComponent* InStaticMesh, float InDeltaTime);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> MainMesh = nullptr;

    UPROPERTY(EditAnywhere)
    float MoveSpeed = 10.0f;

    float CurrentDegree = 0.0f;
    float Period = 2.0f;
    float Distance = 50.0f;
};
