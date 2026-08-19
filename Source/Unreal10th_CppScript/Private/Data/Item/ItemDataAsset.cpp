// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/ItemDataAsset.h"
#include "Engine/AssetManager.h"
#include "Item/PickupBase.h"

TSharedPtr<FStreamableHandle> UItemDataAsset::RequestDataLoad(FStreamableDelegate InDelegate) const
{
    TArray<FSoftObjectPath> TargetsToLoad;
    OnAsyncRequest(TargetsToLoad);

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    return Streamable.RequestAsyncLoad(TargetsToLoad, MoveTemp(InDelegate));
}

bool UItemDataAsset::IsLoaded() const
{
    return PickupClass.IsValid() && (Icon.IsNull() || Icon.IsValid());
}

void UItemDataAsset::OnAsyncRequest(TArray<FSoftObjectPath>& InOutTargetsToLoad) const
{
    InOutTargetsToLoad.Add(PickupClass.ToSoftObjectPath());
    InOutTargetsToLoad.Add(Icon.ToSoftObjectPath());
}
