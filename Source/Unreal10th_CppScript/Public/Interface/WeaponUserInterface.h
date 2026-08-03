// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponUserInterface.generated.h"

class UWeaponComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponUserInterface : public UInterface
{
    GENERATED_BODY()
};

class UNREAL10TH_CPPSCRIPT_API IWeaponUserInterface
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    virtual UWeaponComponent* GetWeaponComponent() const = 0;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void EquipWeapon(UWeaponDataAsset* InWeaponData);

};
