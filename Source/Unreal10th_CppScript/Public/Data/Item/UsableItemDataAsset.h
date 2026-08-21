// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/MiscItemDataAsset.h"
#include "Data/Item/Action/ItemAction.h"
#include "UsableItemDataAsset.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UUsableItemDataAsset : public UMiscItemDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Action")
    TObjectPtr<UItemAction> ItemAction = nullptr;

};
