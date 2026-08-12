// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maze/MazeData.h"
#include "MazeActor.generated.h"

struct FCellData;
class ACellActor;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class UNREAL10TH_CPPSCRIPT_API AMazeActor : public AActor
{
    GENERATED_BODY()

public:
    AMazeActor();

    UFUNCTION(CallInEditor, Category = "Maze")
    void GenerateMaze();

    UFUNCTION(CallInEditor, Category = "Maze")
    void ClearMaze();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    virtual void OnPreMazeGenerate();
    virtual void OnPostMazeGenerate();

private:
    TUniquePtr<FMazeData> MakeMazeData();

    /* CellActor 스폰 방식 */
    void SpawnCells(FMazeData* InMaze);
    void ClearSpawnedCells();
    /***********************/

    /* HISM 사용 방식 */
    void BuildMazeHISM(FMazeData* InMaze);
    void ClearHISMInstances();
    void MakeCellHISM(const FCellData* InCell, const FVector& InLocation);
    /***********************/

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze", meta = (ClampMin = "3", ClampMax = "100"))
    int32 Width = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze", meta = (ClampMin = "3", ClampMax = "100"))
    int32 Height = 3;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze")
    int32 RandomSeed = FMazeData::RandomSeed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze")
    bool bUseHISM = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze")
    EMazeAlgorithmType MazeAlgorithmType = EMazeAlgorithmType::Wilson;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maze|HISM")
    float CellHalfSize = 1000.0f;

    /* CellActor 스폰 방식 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maze|Spawn")
    TSubclassOf<ACellActor> CellActorClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Spawn")
    TArray<TObjectPtr<ACellActor>> CellActors;
    /***********************/

    /* HISM 사용 방식 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> FloorHISM = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> WallHISM = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Maze|Components")
    TObjectPtr<UHierarchicalInstancedStaticMeshComponent> GateHISM = nullptr;
    /***********************/

};
