// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ActionCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

#include "Component/StatComponent.h"
#include "Interface/StaminaInterface.h"
#include "Animation/AnimMontage.h"
#include "AnimNotify/AnimNotifyState_SectionJump.h"
#include "Data/WeaponDataAsset.h"
#include "Weapon/WeaponActor.h"
#include "Kismet/GameplayStatics.h"

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

    bUseControllerRotationYaw = false; // 컨트롤러를 움질일 때 폰이 같이 회전되는 것 방지
    GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터 이동 방향으로 바라보게 만들기
}

UStatComponent* AActionCharacter::GetStatComponent() const
{
    return StatComponent;
}

void AActionCharacter::OnWeaponAttackState(bool bEnable)
{
    OnWeaponAttackStateChanged.ExecuteIfBound(bEnable);
}

void AActionCharacter::OnWeaponDrop(UWeaponDataAsset* InDropWeaponData)
{
    if (DefaultWeaponData && (DefaultWeaponData != InDropWeaponData))
    {
        IWeaponUserInterface::Execute_EquipWeapon(this, DefaultWeaponData);
    }
}

void AActionCharacter::EquipWeapon_Implementation(UWeaponDataAsset* InWeaponData)
{
    // 같은 종류의 무기 장비
    if (InWeaponData == CurrentWeaponData)
    {
        CurrentWeapon->ResetUseCount();
    }
    // 다른 종류의 무기 장비
    else
    {

        // 이전 무기 해제
        IWeaponUserInterface::Execute_UnEquipWeapon(this);

        // 새 무기 장비
        CurrentWeaponData = InWeaponData;

        if (!CurrentWeaponData)
        {
            return;
        }

        // 에셋이 로딩 안됐으면 로딩 요청
        if (!CurrentWeaponData->IsLoaded())
        {
            // 현재 요청을 람다 함수에 캡처하기 위한 변수
            UWeaponDataAsset* RequestedData = CurrentWeaponData;

            // 람다 함수는 로딩이 완료됐을 때 실행된다
            // 만약 로딩 중에 다른 무기를 장착하는 요청이 들어올 경우
            // 장착 요청한 무기와 람다 함수에서 스폰된 무기가 달라진다
            // 따라서 현재 장착 요청한 무기와 캡처해서 넘겼던 무기 데이터가 같을 때만 스폰
            CurrentWeaponData->RequestDataLoad(
                FStreamableDelegate::CreateWeakLambda(
                    this,
                    [this, RequestedData]() {
                        if (CurrentWeaponData == RequestedData)
                        {
                            SpawnWeaponActorAndEquip();
                        }
                    }
                )
            );
        }
        // 에셋이 로딩 된 상태면 바로 무기 소환 및 장착
        else
        {
            SpawnWeaponActorAndEquip();
        }
    }

}

void AActionCharacter::EquipBasicWeapon_Implementation()
{
    if (CurrentWeapon.IsValid())
    {
        CurrentWeapon->DropWeapon();
        CurrentWeapon = nullptr;
    }

    CurrentWeaponData = DefaultWeaponData;

    SpawnWeaponActorAndEquip();
}

void AActionCharacter::UnEquipWeapon_Implementation()
{
    if (CurrentWeapon.IsValid())
    {
        CurrentWeapon->DropWeapon();
        CurrentWeapon = nullptr;
    }
}

void AActionCharacter::SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJumpNotify)
{
    SectionJumpNotify = InSectionJumpNotify;
    bComboReady = SectionJumpNotify.IsValid();
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

    if (DefaultWeaponData)
    {
        IWeaponUserInterface::Execute_EquipWeapon(this, DefaultWeaponData);
        //IWeaponUserInterface::Execute_EquipBasicWeapon(this); // DEPRECATED
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

void AActionCharacter::SectionJumpForCombo()
{
    if (SectionJumpNotify.IsValid() && bComboReady)
    {
        // 콤보로 몽타주가 시작되었다 -> 이전 애니메이션이 끝났다 -> 횟수를 감소시킨다
        OnAttackEnded(nullptr, true);

        UAnimMontage* Current = AnimInstance->GetCurrentActiveMontage();
        AnimInstance->Montage_SetNextSection(
            AnimInstance->Montage_GetCurrentSection(),
            SectionJumpNotify->GetNextSectionName(),
            Current
        );

        FOnMontageEnded AttackEndDelegate;
        AttackEndDelegate.BindUObject(this, &AActionCharacter::OnAttackEnded);
        AnimInstance->Montage_SetEndDelegate(AttackEndDelegate, AttackMontage);

        OnWeaponAttackState(false);
        IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), AttackCost);
        bComboReady = false;
    }
}

void AActionCharacter::SpawnWeaponActorAndEquip()
{
    // 무기 로딩이 끝나기 전에 장착 해제했으면
    // 무기를 스폰할 필요 없음
    if (!CurrentWeaponData)
    {
        return;
    }

    CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(AWeaponActor::StaticClass(), FTransform::Identity, this, this);
    if (CurrentWeapon.IsValid())
    {
        CurrentWeapon->InitializeWeapon(CurrentWeaponData);
        CurrentWeapon->OnWeaponDrop.BindUObject(this, &AActionCharacter::OnWeaponDrop);
        UGameplayStatics::FinishSpawningActor(CurrentWeapon.Get(), FTransform::Identity);
        CurrentWeapon->EquipToTarget(this);
    }
}

void AActionCharacter::OnAttackEnded(UAnimMontage* InMontage, bool bInterrupted)
{
    if (CurrentWeapon.IsValid())
    {
        CurrentWeapon->Use();
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
        EnhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &AActionCharacter::OnAttackAction);
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

float AActionCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (UStatComponent* StatComp = GetStatComponent())
    {
        IHealthInterface::Execute_DamageHealth(StatComp, Damage);

        UE_LOG(LogTemp, Log, TEXT("%.1f 데미지를 입었습니다. (공격자: %s)"), Damage,
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
        && CurrentWeapon.IsValid()
        && CurrentWeapon.Get()->CanUse())
    {
        // 다른 몽타주가 재생중이지 않을 때만 공격
        if (!AnimInstance->IsAnyMontagePlaying())
        {
            // 첫 번째 콤보 공격
            PlayAnimMontage(AttackMontage);

            FOnMontageEnded AttackEndDelegate;
            AttackEndDelegate.BindUObject(this, &AActionCharacter::OnAttackEnded);
            AnimInstance->Montage_SetEndDelegate(AttackEndDelegate, AttackMontage);

            OnWeaponAttackState(false);
            IStaminaInterface::Execute_ConsumeStamina(GetStatComponent(), AttackCost);
        }
        // 공격 중일 때 또 공격하면, 다음 콤보 재생
        else if (AnimInstance->GetCurrentActiveMontage() == AttackMontage)
        {
            SectionJumpForCombo();
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
