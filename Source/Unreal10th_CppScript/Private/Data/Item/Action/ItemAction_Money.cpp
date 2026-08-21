// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Item/Action/ItemAction_Money.h"
#include "Component/InventoryComponent.h"
#include "Component/InventoryCommandTypes.h"
#include "Interface/InventoryUserInterface.h"

void UItemAction_Money::ExecuteAction_Implementation(AActor* InInstigator, AActor* InTarget)
{
    UE_LOG(LogTemp, Log, TEXT("%s에게 돈이 %d 추가됩니다."), *InTarget->GetName(), MoneyAmount);
    if (IInventoryUserInterface* InventoryUser = Cast<IInventoryUserInterface>(InTarget))
    {
        if (UInventoryComponent* InventoryComponent = InventoryUser->GetInventoryComponent())
        {
            FInventoryCommandResult Result;
            InventoryComponent->ExecuteCommand(
                FInventoryCommand::MakeMoneyCommand(MoneyAmount),
                Result
            );
        }
    }
}
