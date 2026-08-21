// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemAction.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class UNREAL10TH_CPPSCRIPT_API UItemAction : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = ItemAction)
    void ExecuteAction(AActor* InInstigator, AActor* InTarget);

    // 언리얼 헤더 툴 제약상 BlueprintNativeEvent는 순수 가상함수 불가능
    // 따라서 기본 바디 필수
    virtual void ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget) {}

};
