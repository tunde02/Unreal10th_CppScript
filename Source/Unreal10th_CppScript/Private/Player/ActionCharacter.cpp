// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActionCharacter.h"

#include "Component/StatComponent.h"
#include "Component/WeaponComponent.h"
#include "Component/InventoryComponent.h"
#include "Interface/StaminaInterface.h"
#include "AnimNotify/AnimNotifyState_SectionJump.h"
#include "Data/Item/WeaponDataAsset.h"
#include "Weapon/WeaponActor.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimMontage.h"

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
    WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

    bUseControllerRotationYaw = false; // 컨트롤러를 움질일 때 폰이 같이 회전되는 것 방지
    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동 방향으로 바라보게 만들기
}

void AActionCharacter::EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData)
{
    if (GetWeaponComponent())
    {
        WeaponComponent->EquipWeapon(InWeaponData);
    }
}

bool AActionCharacter::ExecuteInventoryCommand(const FInventoryCommand& Command, FInventoryCommandResult& OutResult)
{
    if (GetInventoryComponent())
    {
        return InventoryComponent->ExecuteCommand(Command, OutResult);
    }

    return false;
}

UStatComponent* AActionCharacter::GetStatComponent() const
{
    return StatComponent;
}

UWeaponComponent* AActionCharacter::GetWeaponComponent() const
{
    return WeaponComponent;
}

UInventoryComponent* AActionCharacter::GetInventoryComponent() const
{
    return InventoryComponent;
}

void AActionCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (GetCharacterMovement())
    {
        if (WalkSpeed < 0.1f)
        {
            // WalkSpeed를 설정 안했으면 블루프린트에 입력된 기본 걷기 속도로 설정
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

void AActionCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SpendSprintStamina(DeltaTime);
}

void AActionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(IA_Test, ETriggerEvent::Started, this, &AActionCharacter::OnTestAction);
        EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AActionCharacter::OnMoveAction);
        EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &AActionCharacter::OnAttackAction);
        EnhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &AActionCharacter::OnRollAction);
        EnhancedInputComponent->BindAction(IA_Inventory, ETriggerEvent::Started, this, &AActionCharacter::OnInventoryAction);

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

float AActionCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (UStatComponent* StatComp = GetStatComponent())
    {
        IHealthInterface::Execute_DamageHealth(StatComp, Damage);

        UE_LOG(LogTemp, Log, TEXT("%.1f 데미지를 입었습니다. (공격자: %s)"),
               Damage,
               EventInstigator ? *EventInstigator->GetName() : TEXT("알 수 없음"));
    }

    return Damage;
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

void AActionCharacter::OnAttackAction(const FInputActionValue& InValue)
{
    if (AnimInstance
        && IStaminaInterface::Execute_GetCurrentStamina(GetStatComponent()) > AttackCost
        && GetWeaponComponent()
        && GetWeaponComponent()->CanWeaponUse())
    {
        bool bAttack = GetWeaponComponent()->Attack();
        if (bAttack)
        {
            IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), AttackCost);
        }
    }
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

void AActionCharacter::OnInventoryAction(const FInputActionValue& InValue)
{
    if (GetInventoryComponent())
    {
        GetInventoryComponent()->BrodcastOnInventoryAction();
    }
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
