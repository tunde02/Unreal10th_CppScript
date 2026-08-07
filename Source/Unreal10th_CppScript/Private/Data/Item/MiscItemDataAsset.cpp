// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/MiscItemDataAsset.h"

bool UMiscItemDataAsset::IsLoaded() const
{
    return Super::IsLoaded() && Mesh.IsValid();
}

void UMiscItemDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutTargetsToLoad) const
{
    Super::OnAsyncRequest(InOutTargetsToLoad);

    InOutTargetsToLoad.Add(Mesh.ToSoftObjectPath());
}
