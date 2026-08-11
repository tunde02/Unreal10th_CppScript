// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maze/CellData.h"
#include "CellActor.generated.h"

UCLASS()
class UNREAL10TH_CPPSCRIPT_API ACellActor : public AActor
{
    GENERATED_BODY()

public:
    ACellActor();

    // CellData 기반으로 경로 설정하고 문열기
    void InitializeCell(FCellData* InCellData);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // Path 설정에 따라 문이 제대로 열리는지 테스트하는 함수
    UFUNCTION(CallInEditor, Category = "Cell")
    void TestPath();

    // Path 설정에 따라 문을 여는 함수
    void OpenGate();

    // InDirection이 열려 있는지 확인하는 함수
    bool IsPath(EDirectionType InDirection);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
    float CellHalfSize = 1000.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cell")
    float WallHalfThickness = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cell", meta = (Bitmask, BitmaskEnum = "/Script/Unreal10th_CppScript.EDirectionType"))
    int32 Path = 0;
    //EDirectionType Path = EDirectionType::None;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> FloorMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<UStaticMeshComponent>> WallMeshes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<TObjectPtr<UStaticMeshComponent>> GateMeshes;

};
