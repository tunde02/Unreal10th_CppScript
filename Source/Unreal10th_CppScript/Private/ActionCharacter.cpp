// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimMontage.h"
#include "StatComponent.h"
#include "../Interface/StaminaInterface.h"

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

    StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));

    bUseControllerRotationYaw = false; // 컨트롤러를  v xcde43v움질일 때 폰이 같이 회전되는 것 방지
    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동 방향으로 바라보게 만들기
}

UStatComponent* AActionCharacter::GetStatComponent() const
{
    return StatComponent;
}

// Called when the game starts or when spawned
void AActionCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GetCharacterMovement())
    {
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

    if (GetMesh())
    {
        AnimInstance = GetMesh()->GetAnimInstance();
    }

    if (StatComponent)
    {
        FAutoRecoveryData Data = FAutoRecoveryData(
            StaminaAutoRecoveryCoolTime,
            StaminaAutoRecoveryInterval,
            StaminaAutoRecoveryAmountPerTick);
        StatComponent->Initialize(Data);
    }
}

// Called every frame
void AActionCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SpendSprintStamina(DeltaTime);

    // DEPRECATED: 타이머로 대체
    //StaminaAutoRecovery(DeltaTime);
}

void AActionCharacter::SpendSprintStamina(float DeltaTime)
{
    // 달리기 모드
    // 이동 하고 있고
    // 몽타주 재생 중이 아니라면
    if (bSprintMode && !GetVelocity().IsNearlyZero()
        && (AnimInstance && !AnimInstance->IsAnyMontagePlaying()))
    {
        if (!IStaminaInterface::Execute_ConsumeStamina(StatComponent, SprintStaminaCostPerSecond * DeltaTime))
        {
            // 스태미나가 다 떨어지면 달리기 모드 정지
            OnSprintEnd();
        }
    }
}

// Called to bind functionality to input
void AActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(IA_Test, ETriggerEvent::Started, this, &AActionCharacter::OnTestAction);
        EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AActionCharacter::OnMoveAction);
        EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &AActionCharacter::OnRollAction);

        EnhancedInputComponent->BindActionValueLambda(
            IA_Sprint,
            ETriggerEvent::Started,
            [this](const FInputActionValue& _) {
                OnSprintStart();
            });
        EnhancedInputComponent->BindActionValueLambda(
            IA_Sprint,
            ETriggerEvent::Completed,
            [this](const FInputActionValue& _) {
                OnSprintEnd();
            });
    }
}

void AActionCharacter::OnTestAction(const FInputActionValue& InValue)
{
    UE_LOG(LogTemp, Log, TEXT("TestAction 실행"));
}

void AActionCharacter::OnMoveAction(const FInputActionValue& InValue)
{
    FVector2D Input = InValue.Get<FVector2D>();
    FVector Direction = FVector(Input.X, Input.Y, 0.0f).GetSafeNormal();

    float YawRadian =FMath::DegreesToRadians(GetControlRotation().Yaw);
    FQuat ControlYawRotation(FVector::UpVector, YawRadian);

    // 카메라 Yaw 회전 만큼 Direction을 회전
    Direction = ControlYawRotation.RotateVector(Direction);

    AddMovementInput(Direction);
}

void AActionCharacter::OnRollAction(const FInputActionValue& InValue)
{
    if (!RollMontage)
    {
        return;
    }

    if (!AnimInstance)
    {
        AnimInstance = GetMesh()->GetAnimInstance();
    }

    if (IStaminaInterface::Execute_ConsumeStamina(StatComponent, RollStaminaCost))
    {
        // 다른 몽타주가 재생 중이지 않을 때만 몽타주 재생
        if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
        {
            // 이동 입력 중이면
            if (!GetLastMovementInputVector().IsNearlyZero())
            {
                // 입력 방향으로 캐릭터를 즉시 회전
                SetActorRotation(GetLastMovementInputVector().Rotation());
            }

            PlayAnimMontage(RollMontage);
        }
    }
}

void AActionCharacter::OnSprintStart()
{
    bSprintMode = true;

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * 2.0f;
}

void AActionCharacter::OnSprintEnd()
{
    bSprintMode = false;

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
