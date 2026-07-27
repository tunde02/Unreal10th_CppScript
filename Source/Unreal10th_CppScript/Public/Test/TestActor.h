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

    void Test_NormalFunction();

    // C++로 작성한 함수를 블루프린트에서 호출할 수 있다
    UFUNCTION(BlueprintCallable)
    void Test_UFunction();

    // C++에서 선언만 하고 구현은 블루프린트에서 하겠다
    // C++에 구현부가 있으면 안됨!
    // 블루프린트 코드를 써야만 하는 상황일 때 사용
    UFUNCTION(BlueprintImplementableEvent)
    void Test_ImplementableFunction();

    // C++에서 선언과 구현을 했는데, 블루프린트에서 재정의 할 수도 있다
    // 구현부엔 "_Implementation" 접미사를 붙여야함. 호출은 함수 이름 그대로
    // 블루프린트에서 사용할 땐 함수 오버라이드로 불러와야됨
    UFUNCTION(BlueprintNativeEvent)
    void Test_NativeEventFunction();

protected:
    int32 Data1 = 10;	// C++ 전용 변수.

    // UPROPERTY : 클래스나 구조체의 멤버 변수를 언리얼 엔진에 등록하는 것 (리플렉션, 가비지 콜렉팅 지원)

    // 클래스 디폴트 창과, 맵에 배치된 액터의 디테일 창에서 확인 가능
    UPROPERTY(VisibleAnywhere, Category="TestActor변수|Visible")
    int32 Data2_1 = 21;

    // 클래스 디폴트 창에서만 보임
    UPROPERTY(VisibleDefaultsOnly, Category="TestActor변수|Visible")
    int32 Data2_2 = 22;

    // 맵에 배치된 액터의 디테일 창에서만 보임
    UPROPERTY(VisibleInstanceOnly, Category="TestActor변수|Visible")
    int32 Data2_3 = 23;

    // 클래스 디폴트 창과, 맵에 배치된 액터의 디테일 창에서 수정 가능
    UPROPERTY(EditAnywhere, Category="TestActor변수|Editable")
    int32 Data3_1 = 31;

    // 클래스 디폴트 창에서만 수정 가능
    UPROPERTY(EditDefaultsOnly, Category="TestActor변수|Editable")
    int32 Data3_2 = 32;

    // 맵에 배치된 액터의 디테일 창에서만 수정 가능
    UPROPERTY(EditInstanceOnly, Category="TestActor변수|Editable")
    int32 Data3_3 = 33;

    // 블루프린트 이벤트 그래프에서 읽을 수 있다
    UPROPERTY(BlueprintReadOnly, Category="TestActor변수|Read")
    int32 Data4_1 = 41;

    // 블루프린트 이벤트 그래프에서 읽을 수 있고 디테일 창에서 편집도 가능
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="TestActor변수|Read")
    int32 Data4_2 = 42;

    // 블루프린트 이벤트 그래프에서 읽고 쓸 수 있다
    UPROPERTY(BlueprintReadWrite, Category="TestActor변수|ReadWrite")
    int32 Data4_3 = 43;

    // 블루프린트 이벤트 그래프에서 읽고 쓸 수 있다. 그리고 디테일 창에서 편집도 가능
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="TestActor변수|ReadWrite")
    int32 Data4_4 = 44;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> MainMesh = nullptr;
};
