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
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    //virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    // 오버랩 델리게이트에 바인딩할 함수
    UFUNCTION()
    void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

    virtual void OnPickup(AActor* InTarget);
    virtual void OnUpdatePickupEffect();
    virtual void OnFinishPickupEffect();
    virtual void OnUpdateUpDownSpin(float InDeltaTime);
    virtual UMeshComponent* GetMesh() const;

private:
    bool IsCurveAssetReady() const;
    bool IsPickupEffectAssetReady() const;

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

    // 스폰 직후에 아이템이 안먹어지는 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Data")
    float PickupDelayTime = 1.0f;

    // 맵에 있을 때 위아래로 왕복하는 모습용 커브
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Spawn")
    TObjectPtr<UCurveFloat> UpDownCurve;

    // 맵에 있을 때 회전하는 모습용 커브
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Spawn")
    TObjectPtr<UCurveFloat> SpinCurve;

    // 위아래로 왕복하는데 걸리는 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Spawn")
    float UpDownDuration = 2.0f;

    // 위아래로 움직이는 거리
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Spawn")
    float UpDownHeight = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    TObjectPtr<UCurveFloat> PickupAlpha;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    TObjectPtr<UCurveFloat> PickupHeight;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    TObjectPtr<UCurveFloat> PickupScale;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    float PickupEffectDuration = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect|Pickup")
    float PickupEffectHeight = 50.0f;

    TWeakObjectPtr<AActor> TargetActor = nullptr;
    FTimerHandle PickupEffectTimerHandle;

private:
    bool bIdle = true;
    float ElapsedTime = 0.0f;
    FVector MeshZOffset = FVector(0.0f, 0.0f, -10.f);

    const float TimerInterval = 0.02f; // 아이템을 줍는 연출용 타이머의 실행 간격
    float PickupElapsedTime = 0.0f;
    FVector PickupStartLocation;

};
