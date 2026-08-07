// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/ItemDataAsset.h"
#include "MiscItemDataAsset.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UMiscItemDataAsset : public UItemDataAsset
{
    GENERATED_BODY()

public:
    virtual bool IsLoaded() const override;

protected:
    virtual void OnAsyncRequest(TArray<FSoftObjectPath>& InOutTargetsToLoad) const override;

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Misc|Appearance")
    TSoftObjectPtr<UStaticMesh> Mesh;

};
