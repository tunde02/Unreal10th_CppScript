// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Data/Item/ItemDataAsset.h"
#include "InventoryDragDropOperation.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UInventoryDragDropOperation : public UDragDropOperation
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadonly)
    int32 SourceIndex = -1;

};
