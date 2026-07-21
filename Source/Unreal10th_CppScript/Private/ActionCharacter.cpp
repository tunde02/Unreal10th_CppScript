// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AActionCharacter::AActionCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    CameraSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
    CameraSpringArmComponent->SetupAttachment(RootComponent);
    CameraSpringArmComponent->bUsePawnControlRotation = true; // 스프링암은 컨트롤러 입력에 맞게 회전됨

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
    CameraComponent->SetupAttachment(CameraSpringArmComponent);

    bUseControllerRotationYaw = false; // 컨트롤러를 움질일 때 폰이 같이 회전되는 것 방지
    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동 방향으로 바라보게 만들기
}

// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (WalkSpeed < 0.1f)
    {
        // WalkSpeed를 설정 안했으면 블루프린트에 입력된 기본 걷기 속도
        WalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
    }
    else
    {
        // WalkSpeed를 설정 했으면 그 값으로 설정
        GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    }
}

// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(IA_Test, ETriggerEvent::Started, this, &AActionCharacter::OnTestAction);
        EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AActionCharacter::OnMoveAction);
        EnhancedInputComponent->BindAction(IA_Dash, ETriggerEvent::Started, this, &AActionCharacter::OnStartDashAction);
        EnhancedInputComponent->BindAction(IA_Dash, ETriggerEvent::Completed, this, &AActionCharacter::OnEndDashAction);
    }
}

void AActionCharacter::OnTestAction(const FInputActionValue& InValue)
{
    UE_LOG(LogTemp, Log, TEXT("TestAction 실행"));
}

void AActionCharacter::OnMoveAction(const FInputActionValue& InValue)
{
    FVector2D LookAxis = InValue.Get<FVector2D>();
    FVector Direction(LookAxis.X, LookAxis.Y, 0.0f);

    Direction = GetControlRotation().RotateVector(Direction);

    AddMovementInput(Direction);
}

void AActionCharacter::OnStartDashAction(const FInputActionValue& InValue)
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * 2.0f;
    UE_LOG(LogTemp, Log, TEXT("OnStartDashAction : %f"), GetCharacterMovement()->MaxWalkSpeed);
}

void AActionCharacter::OnEndDashAction(const FInputActionValue& InValue)
{
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
    UE_LOG(LogTemp, Log, TEXT("OnEndDashAction : %f"), GetCharacterMovement()->MaxWalkSpeed);
}
