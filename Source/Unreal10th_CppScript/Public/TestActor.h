// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// generated.h는 모든 해더 중 마지막에 있어야 한다.
#include "TestActor.generated.h"

// UCLASS()
//	- C++ 클래스를 언리얼 엔진의 객체 시스템에 등록(= UObject)
//	- 가비지 콜랙팅 지원, 블루프린트 연동, 에디터서 노출 등이 가능해짐
//	- 다중 상속을 막는다.

// CDO (Class Default Object)
//	- 기본 클래스 인스턴스. 새 클래스가 인스턴싱될 때 CDO를 클로닝해서 사용.


UCLASS()
class UNREAL10TH_CPPSCRIPT_API ATestActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ATestActor();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    int32 Data1 = 10;	// C++ 전용 변수.

    // UPROPERTY : 클래스나 구조체의 멤버 변수를 언리얼 엔진에 등록하는 것 (리플렉션, 가비지 콜렉팅 지원)

    // 클래스 디폴트 창과, 맵에 배치된 액터의 디테일 창에서 확인 가능
    UPROPERTY(VisibleAnywhere)
    int32 Data2_1 = 21;

    // 클래스 디폴트 창에서만 보임
    UPROPERTY(VisibleDefaultsOnly)
    int32 Data2_2 = 22;

    // 맵에 배치된 액터의 디테일 창에서만 보임
    UPROPERTY(VisibleInstanceOnly)
    int32 Data2_3 = 23;

    // 클래스 디폴트 창과, 맵에 배치된 액터의 디테일 창에서 수정 가능
    UPROPERTY(EditAnywhere)
    int32 Data3_1 = 31;

    // 클래스 디폴트 창에서만 수정 가능
    UPROPERTY(EditDefaultsOnly)
    int32 Data3_2 = 31;

    // 맵에 배치된 액터의 디테일 창에서만 수정 가능
    UPROPERTY(EditInstanceOnly)
    int32 Data3_3 = 31;
};
