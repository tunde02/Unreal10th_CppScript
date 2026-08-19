// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PickupBase.h"
#include "PickupMisc.generated.h"

class UMiscItemDataAsset;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API APickupMisc : public APickupBase
{
    GENERATED_BODY()

public:
    APickupMisc();

    virtual void InitializePickup(UItemDataAsset* InData) override;

protected:
    //virtual void OnConstruction(const FTransform& Transform) override;
    virtual UMeshComponent* GetMesh() const override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

private:
    TWeakObjectPtr<UMiscItemDataAsset> MiscData = nullptr;
};
