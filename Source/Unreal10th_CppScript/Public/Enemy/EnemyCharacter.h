// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/StatInterface.h"
#include "EnemyCharacter.generated.h"

class UStatComponent;
class UAnimMontage;
class APickupWeapon;
class ADamagePopupActor;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AEnemyCharacter : public ACharacter, public IStatInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AEnemyCharacter();

    UFUNCTION(BlueprintCallable)
    virtual UStatComponent* GetStatComponent() const override;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION()
    virtual void OnDie();

    UFUNCTION()
    virtual void DropItems();

private:
    void HandleOnDie(UAnimMontage* InMontage, bool bInterrupted);
    void HandlePickupItemBounce(AActor* InActor) const;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStatComponent> StatComp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UDataTable> ItemDropTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Action Anims")
    TObjectPtr<UAnimMontage> DieMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Popup Class")
    TSubclassOf<ADamagePopupActor> DamagePopupClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Weapon Class")
    TSubclassOf<APickupWeapon> PickupWeaponClass;

};
