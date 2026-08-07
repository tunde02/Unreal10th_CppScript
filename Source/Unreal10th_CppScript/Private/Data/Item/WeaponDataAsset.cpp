// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/WeaponDataAsset.h"
#include "Engine/AssetManager.h"

bool UWeaponDataAsset::IsLoaded() const
{
    return Super::IsLoaded() && Mesh.IsValid() && TrailVfx.IsValid() && WeaponHitVfx.IsValid();
}

void UWeaponDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutTargetsToLoad) const
{
    Super::OnAsyncRequest(InOutTargetsToLoad);

    InOutTargetsToLoad.Add(Mesh.ToSoftObjectPath());
    InOutTargetsToLoad.Add(TrailVfx.ToSoftObjectPath());
    InOutTargetsToLoad.Add(WeaponHitVfx.ToSoftObjectPath());
}
