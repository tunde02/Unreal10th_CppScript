// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventorySlotWidget.h"
#include "Widget/Inventory/InventoryDragDropOperation.h"
#include "Widget/Inventory/TemporarySlotWidget.h"
#include "Widget/Inventory/InventoryWidget.h"
#include "Component/InventoryComponent.h"

#include "Components/Image.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Blueprint/SlateBlueprintLibrary.h"

void UInventorySlotWidget::InitializeSlot(UInventoryComponent* InInventoryComponent, int32 InSlotIndex)
{
    if (!InInventoryComponent)
    {
        return;
    }

    TargetInventory = InInventoryComponent;
    Index = InSlotIndex;

    RefreshSlot();
}

void UInventorySlotWidget::RefreshSlot() const
{
    if (!TargetInventory.IsValid())
    {
        return;
    }

    const FInvenSlot* TargetSlot = TargetInventory->GetSlot(Index);
    if (!TargetSlot)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Slot %d]가 유효하지 않습니다."), Index);
        return;
    }

    if (TargetSlot->IsEmpty())
    {
        IconImage->SetBrushFromTexture(nullptr);
        IconImage->SetBrushTintColor(FLinearColor::Transparent);
        CountBox->SetVisibility(ESlateVisibility::Hidden);
    }
    else
    {
        IconImage->SetBrushFromTexture(TargetSlot->ItemData->Icon.Get());
        IconImage->SetBrushTintColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
        CountText->SetText(FText::AsNumber(TargetSlot->GetCount()));
        MaxStackText->SetText(FText::AsNumber(TargetSlot->ItemData->MaxStackCount));
        CountBox->SetVisibility(ESlateVisibility::HitTestInvisible);
    }
}

void UInventorySlotWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

    OnSlotEnter.Broadcast(Index);
}

void UInventorySlotWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    OnSlotLeave.Broadcast();

    Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    /*
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        // 중요: 드래그 감지 예약을 걸어둡니다.
        // 유저가 움직이면 DragDetected로 가고, 안 움직이고 떼면 MouseButtonUp으로 갑니다.
        return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
    }

    return FReply::Unhandled();
    */

    if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        if (FInvenSlot* InvenSlot = TargetInventory->GetSlot(Index))
        {
            if (!InvenSlot->IsEmpty())
            {
                return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
            }
        }
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UInventorySlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply Reply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

    if (!TargetInventory.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : InventoryComponent nullptr"));
        return Reply;
    }

    if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
    {
        FInventoryCommandResult Result;
        TargetInventory->ExecuteCommand(
            FInventoryCommand::MakeUseCommand(Index),
            Result
        );
        TargetInventory->ExecuteCommand(
            FInventoryCommand::MakeEquipCommand(Index),
            Result
        ); // ..;? 어차피 Handle 함수들에서 타입 맞지 않으면 리턴하니까 상관은 없다곤 함
    }

    return Reply;
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    if (!TargetInventory.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : InventoryComponent nullptr"));
        return;
    }

    FInvenSlot* InvenSlot = TargetInventory->GetSlot(Index);
    if (!InvenSlot || InvenSlot->IsEmpty())
    {
        return;
    }

    UInventoryDragDropOperation* DragOp = NewObject<UInventoryDragDropOperation>();
    DragOp->SourceIndex = Index;

    UTemporarySlotWidget* DragTempWidget = CreateWidget<UTemporarySlotWidget>(
        this,
        TargetInventory->GetTemporarySlotWidgetClass()
    );
    DragTempWidget->InitializeSlot(TargetInventory->GetSlot(Index));
    DragTempWidget->SetVisual(InvenSlot->ItemData->Icon.Get(), InvenSlot->GetCount());

    DragOp->DefaultDragVisual = DragTempWidget; // 얘 전용 레이어가 따로 생겼다가 드래그가 끝나면 사라짐. 따라서 AddToViewport 안해줘도 됨

    OutOperation = DragOp; // NativeOnDrop과 NariveOnDragCancelled를 발동시키기 위해 필수

    FInventoryCommandResult Result;
    TargetInventory->ExecuteCommand(
        FInventoryCommand::MakeMoveCommand(Index, TargetInventory->GetTempSlotIndex()),
        Result);
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    // false 리턴하면 OnDragCancelled 실행해버림
    if (!TargetInventory.IsValid() || TargetInventory->GetTempSlot()->IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("[InventorySlotWidget] : InventoryComponent nullptr OR TemporarySlot Empty"));
        return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
    }

    FInventoryCommandResult Result;
    TargetInventory->ExecuteCommand(
        FInventoryCommand::MakeMoveCommand(TargetInventory->GetTempSlotIndex(), Index),
        Result);

    UInventoryDragDropOperation* DragOp = Cast<UInventoryDragDropOperation>(InOperation);
    TargetInventory->ExecuteCommand(
        FInventoryCommand::MakeMoveCommand(TargetInventory->GetTempSlotIndex(), DragOp->SourceIndex),
        Result);

    return true;
}

void UInventorySlotWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    /*
    FInventoryCommandResult Result;
    TargetInventory->ExecuteCommand(
        FInventoryCommand::MakeDropCommand(TargetInventory->GetTempSlotIndex(), GetOwningPlayerPawn()->GetActorLocation()),
        Result);
    */

    if (APlayerController* PC = GetOwningPlayer())
    {
        UE_LOG(LogTemp, Log, TEXT("플레이어 컨트롤러 확인"));
        //FHitResult HitResult;
        //if (PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult))
        //{
        //	UE_LOG(LogTemp, Log, TEXT("바닥 히트 성공"));

        //	FInventoryCommandResult Result;
        //	TargetInventory->ExecuteCommand(
        //		FInventoryCommand::MakeDrop(TargetInventory->GetTempSlotIndex(), HitResult.Location), 
        //		Result);
        //}

        FVector2D AbsolutePosition = InDragDropEvent.GetScreenSpacePosition();
        FVector2D PixelPosion;
        FVector2D ViewportPosition;
        USlateBlueprintLibrary::AbsoluteToViewport(this, AbsolutePosition, PixelPosion, ViewportPosition);

        FVector WorldLocation;
        FVector WorldDirection;
        if (PC->DeprojectScreenPositionToWorld(
            PixelPosion.X, PixelPosion.Y,
            WorldLocation, WorldDirection))
        {
            FVector Start = WorldLocation;
            FVector End = Start + WorldDirection * 10000.0f;

            FHitResult HitResult;
            FVector SpawnLocation;
            if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility))
            {
                SpawnLocation = HitResult.Location;
            }
            else
            {
                SpawnLocation = End;
            }

            if (APawn* PlayerPawn = PC->GetPawn())
            {
                const float MaxDistance = 500.0f;
                FVector PlayerLocation = PlayerPawn->GetActorLocation();

                if (FVector::DistSquared2D(SpawnLocation, PlayerLocation) > MaxDistance * MaxDistance)
                {
                    FVector Direction = (SpawnLocation - PlayerLocation).GetUnsafeNormal2D();
                    SpawnLocation = PlayerLocation + Direction * MaxDistance;

                    FVector DownStart = SpawnLocation + FVector::UpVector * 10000.0f;
                    FVector DownEnd = SpawnLocation + FVector::DownVector * 10000.0f;
                    FHitResult GroundHit;

                    if (GetWorld()->LineTraceSingleByChannel(GroundHit, DownStart, DownEnd, ECollisionChannel::ECC_Visibility))
                    {
                        SpawnLocation = GroundHit.Location;
                    }
                }
            }

            FInventoryCommandResult Result;
            TargetInventory->ExecuteCommand(
                FInventoryCommand::MakeDropCommand(TargetInventory->GetTempSlotIndex(), SpawnLocation),
                Result);
        }
    }

    Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}
