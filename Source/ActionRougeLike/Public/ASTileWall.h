// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASTileWall.generated.h"

class ASTile;

UCLASS()
class ACTIONROUGELIKE_API ASTileWall : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Sets default values for this actor's properties
	ASTileWall();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void RemoveOuter();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void RemoveInner();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ---------------------------------
	// ------- Public Variables --------
	// ---------------------------------

	//will store properties such as corresponding door, key, etc or other things wall related
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Walls")
	int WallsBuffer; //inner is moved in 100, outer is moved out 100, center still at 0,0,120

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallSetup")
	bool isConnector = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallSetup")
	TArray <UStaticMeshComponent*> WallComponentsInnerArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallSetup")
	TArray <UStaticMeshComponent*> WallComponentsOuterArray;

	//the model is assigned and choosen from local level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Walls")
	UStaticMeshComponent* OuterWallObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Walls")
	UStaticMeshComponent* InnerWallObject;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Walls")
	ASTile* InnerTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Walls")
	ASTile* OuterTile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Walls")
	bool WillRemoveOuter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Walls")
	bool WillRemoveInner;

};
