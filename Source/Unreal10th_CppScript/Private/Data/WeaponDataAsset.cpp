// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/WeaponDataAsset.h"
#include "Engine/AssetManager.h"

TSharedPtr<FStreamableHandle> UWeaponDataAsset::RequestDataLoad(FStreamableDelegate InDelegate)
{
    TArray<FSoftObjectPath> TargetsToLoad;
    TargetsToLoad.Add(Mesh.ToSoftObjectPath());
    TargetsToLoad.Add(TrailVfx.ToSoftObjectPath());
    TargetsToLoad.Add(WeaponHitVfx.ToSoftObjectPath());

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    return Streamable.RequestAsyncLoad(TargetsToLoad, MoveTemp(InDelegate));
}

bool UWeaponDataAsset::IsLoaded() const
{
    return Mesh.IsValid() && TrailVfx.IsValid() && WeaponHitVfx.IsValid();
}
