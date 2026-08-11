// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MazeActor.generated.h"

class ACellActor;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AMazeActor : public AActor
{
    GENERATED_BODY()

public:
    AMazeActor();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

protected:
    UFUNCTION(CallInEditor, Category = "Maze")
    void GenerateMaze();

    UFUNCTION(CallInEditor, Category = "Maze")
    void RemoveMaze();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze")
    int32 Width = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze")
    int32 Height = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze")
    int32 Seed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze")
    TSubclassOf<ACellActor> CellActorClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze")
    TArray<TObjectPtr<ACellActor>> CellActors;

};
