// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UNiagaraComponent;
class UItemDataAsset;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API APickupBase : public AActor
{
    GENERATED_BODY()

public:
    APickupBase();

    virtual void InitializePickup(UItemDataAsset* InData);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    virtual void OnPickup(AActor* InTarget);
    virtual void OnUpdateUpDownSpin(float InDeltaTime);
    virtual UMeshComponent* GetMesh() const;

private:
    bool IsCurveAssetReady() const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<USphereComponent> SphereCollision = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UNiagaraComponent> NiagaraComponent = nullptr;

protected:
    // 메시의 기본 위치
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data")
    FVector MeshBaseLocation = FVector(0.0f, 0.0f, 0.0f);

    // 픽업 시 획득할 데이터 에셋
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data")
    TObjectPtr<UItemDataAsset> DataAsset;

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
    bool bIdle = true;
    float ElapsedTime = 0.0f;
    FVector MeshZOffset = FVector(0.0f, 0.0f, -10.f);

};
