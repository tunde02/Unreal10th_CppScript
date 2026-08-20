// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActionPlayerController.h"

#include "Blueprint/UserWidget.h"

// 입력 받기 위해 추가한 헤더
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"

void AActionPlayerController::OnInventoryOpenClose(bool bInventoryOpen, UUserWidget* InFocusWidget)
{
    if (bInventoryOpen)
    {
        FInputModeUIOnly InputModeUI;
        InputModeUI.SetWidgetToFocus(InFocusWidget->TakeWidget());

        SetInputMode(InputModeUI);
        SetShowMouseCursor(true);
    }
    else
    {
        FInputModeGameOnly InputModeGame;
        SetInputMode(InputModeGame);
        SetShowMouseCursor(false);
    }
}

void AActionPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 로컬 플레이어의 SubSystem을 불러온다
    UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

    if (SubSystem && DefaultMappingContext)
    {
        SubSystem->AddMappingContext(DefaultMappingContext, GameInputPriority);
    }

    if (PlayerCameraManager)
    {
        PlayerCameraManager->ViewPitchMin = ViewPitchMin;
        PlayerCameraManager->ViewPitchMax = ViewPitchMax;
    }
}

void AActionPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // 컨트롤러라서 자기가 InputComponent를 가지고 있으므로 바로 캐스팅해서 사용
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AActionPlayerController::OnLookInput);
    }
}

void AActionPlayerController::OnLookInput(const FInputActionValue& InValue)
{
    FVector2D LookAxis = InValue.Get<FVector2D>();

    AddYawInput(LookAxis.X);
    AddPitchInput(LookAxis.Y);
}
