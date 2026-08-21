// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/Item/Action/ItemAction.h"
#include "ItemAction_StaminaRecovery.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API UItemAction_StaminaRecovery : public UItemAction
{
    GENERATED_BODY()

public:
    virtual void ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget) override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadonly, Category = "Stamina")
    float StaminaAmount = 50.0f;

};
