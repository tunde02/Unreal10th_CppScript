// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolableInterface : public UInterface
{
    GENERATED_BODY()
};

class UNREAL10TH_CPPSCRIPT_API IPoolableInterface
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    // 풀링되는 액터의 사용이 끝나서 풀로 되돌리는 작업 진행
    virtual void UseFinish() = 0;

    // 스폰될 때 실행해야 할 작업들 처리
    UFUNCTION(BlueprintNativeEvent)
    void OnSpawn();

    // 풀로 되돌아갈 때 실행해야 할 작업들 처리
    UFUNCTION(BlueprintNativeEvent)
    void OnReturn();

};
