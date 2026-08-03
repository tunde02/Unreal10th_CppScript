// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WeaponComponent.h"
#include "Unreal10th_CppScript/Unreal10th_CppScript.h"
#include "AnimNotify/AnimNotifyState_SectionJump.h"
#include "Data/WeaponDataAsset.h"
#include "Weapon/WeaponActor.h"
#include "Interface/WeaponUserInterface.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

UWeaponComponent::UWeaponComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;

    // ...
}

bool UWeaponComponent::Attack()
{
    if (!OwnerCharacter.IsValid() || !OwnerAnimInstance.IsValid())
    {
        return false;
    }

    bool bResult = false;

    if (!OwnerAnimInstance->IsAnyMontagePlaying())
    {
        // 첫번째 콤보 공격
        OwnerCharacter->PlayAnimMontage(CurrentWeaponData->AttackMontage);

        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &UWeaponComponent::OnAttackEnded);
        OwnerAnimInstance->Montage_SetEndDelegate(EndDelegate, CurrentWeaponData->AttackMontage);

        OnWeaponAttackState(false);

        bResult = true;
    }
    else if (OwnerAnimInstance->GetCurrentActiveMontage() == CurrentWeaponData->AttackMontage)
    {
        bResult = SectionJumpForCombo();
    }

    return bResult;
}

void UWeaponComponent::AreaAttack()
{
    if (!CurrentWeaponData || !CurrentWeapon.IsValid() || !OwnerCharacter.IsValid())
    {
        return;
    }

    // 디버그 정보 출력
    DrawDebugSphere(
        GetWorld(),
        CurrentWeapon->GetWeaponImpactLocation(),
        CurrentWeaponData->AreaAttackInnerRadius,
        12,
        FColor::Red,
        false,
        5.0f
    );
    DrawDebugSphere(
        GetWorld(),
        CurrentWeapon->GetWeaponImpactLocation(),
        CurrentWeaponData->AreaAttackOuterRadius,
        12,
        FColor::Yellow,
        false,
        5.0f
    );

    TArray<AActor*> IgnoreActors = { CurrentWeapon.Get(), OwnerCharacter.Get() };
    UGameplayStatics::ApplyRadialDamageWithFalloff(
        GetWorld(),
        CurrentWeaponData->AreaAttackPower,
        1.0f,
        CurrentWeapon->GetWeaponImpactLocation(),
        CurrentWeaponData->AreaAttackInnerRadius,
        CurrentWeaponData->AreaAttackOuterRadius,
        1.0f,
        nullptr,
        IgnoreActors,
        CurrentWeapon.Get(),
        OwnerCharacter->GetController(),
        ECC_Enemy
    );
}

void UWeaponComponent::Initialize()
{
    if (DefaultWeaponData)
    {
        EquipWeapon(DefaultWeaponData);
    }
}

void UWeaponComponent::EquipWeapon(UWeaponDataAsset* InWeaponData)
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
        if (CurrentWeapon.IsValid())
        {
            CurrentWeapon->DropWeapon();
            CurrentWeapon = nullptr;
        }

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
        // 에셋이 로딩된 상태면 바로 무기 소환 및 장착
        else
        {
            SpawnWeaponActorAndEquip();
        }
    }
}

void UWeaponComponent::OnWeaponDrop(UWeaponDataAsset* InDropWeaponData)
{
    if (DefaultWeaponData && (DefaultWeaponData != InDropWeaponData))
    {
        EquipWeapon(DefaultWeaponData);
    }
}

void UWeaponComponent::OnWeaponAttackState(bool bEnable)
{
    OnWeaponAttackStateChanged.ExecuteIfBound(bEnable);
}

bool UWeaponComponent::CanWeaponUse() const
{
    return CurrentWeapon.IsValid() && CurrentWeapon->CanUse();
}

void UWeaponComponent::SetSectionJumpNotify(UAnimNotifyState_SectionJump* InSectionJumpNotify)
{
    SectionJumpNotify = InSectionJumpNotify;
    bComboReady = SectionJumpNotify.IsValid();
}

void UWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ACharacter>(GetOwner());

    if (OwnerCharacter.IsValid())
    {
        if (OwnerCharacter->GetMesh())
        {
            OwnerAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
        }
    }

    if (DefaultWeaponData)
    {
        IWeaponUserInterface::Execute_EquipWeapon(OwnerCharacter.Get(), DefaultWeaponData);
    }
}

void UWeaponComponent::SpawnWeaponActorAndEquip()
{
    // 무기 로딩이 끝나기 전에 장착 해제했으면
    // 무기를 스폰할 필요 없음
    if (!CurrentWeaponData || !OwnerCharacter.IsValid())
    {
        return;
    }

    CurrentWeapon = GetWorld()->SpawnActorDeferred<AWeaponActor>(AWeaponActor::StaticClass(), FTransform::Identity, OwnerCharacter.Get(), OwnerCharacter.Get());
    if (CurrentWeapon.IsValid())
    {
        CurrentWeapon->InitializeWeapon(CurrentWeaponData);
        CurrentWeapon->OnWeaponDrop.BindUObject(this, &UWeaponComponent::OnWeaponDrop);
        UGameplayStatics::FinishSpawningActor(CurrentWeapon.Get(), FTransform::Identity);
        CurrentWeapon->EquipToTarget(GetOwner());
    }
}

bool UWeaponComponent::SectionJumpForCombo()
{
    if (!OwnerAnimInstance.IsValid())
    {
        return false;
    }

    bool bResult = false;

    if (SectionJumpNotify.IsValid() && bComboReady)
    {
        // 콤보로 몽타주가 시작되었다 -> 이전 애니메이션이 끝났다 -> 횟수를 감소시킨다
        OnAttackEnded(nullptr, true);

        UAnimMontage* Current = OwnerAnimInstance->GetCurrentActiveMontage();
        OwnerAnimInstance->Montage_SetNextSection(
            OwnerAnimInstance->Montage_GetCurrentSection(),
            SectionJumpNotify->GetNextSectionName(),
            Current
        );

        FOnMontageEnded EndDelegate;
        EndDelegate.BindUObject(this, &UWeaponComponent::OnAttackEnded);
        OwnerAnimInstance->Montage_SetEndDelegate(EndDelegate, CurrentWeaponData->AttackMontage);

        OnWeaponAttackState(false);
        bComboReady = false;

        bResult = true;
    }

    return bResult;
}

void UWeaponComponent::OnAttackEnded(UAnimMontage* InMontage, bool bInterrupted)
{
    if (CurrentWeapon.IsValid())
    {
        CurrentWeapon->Use();
    }
}
