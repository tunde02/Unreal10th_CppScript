// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API APickupBase : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APickupBase();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    virtual void OnPickup(AActor* InTarget);

    virtual void OnUpdateUpDownSpin(float InDeltaTime);

private:
    bool IsCurveAssetReady() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USphereComponent> SphereCollision = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;

protected:
    // 메시의 기본 위치
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data")
    FVector MeshBaseLocation = FVector(0, 0, 50.0f);

    // 맵에 있을 때 위아래로 왕복하는 모습용 커브
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Default")
    TObjectPtr<UCurveFloat> UpDownCurve;

    // 맵에 있을 때 회전하는 모습용 커브
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Default")
    TObjectPtr<UCurveFloat> SpinCurve;

    // 위아래로 왕복하는데 걸리는 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Default")
    float UpDownDuration = 2.0f;

    // 위아래로 움직이는 거리
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Default")
    float UpDownHeight = 100.0f;

private:
    float ElapsedTime = 0.0f;
    bool bIdle = true;

};
