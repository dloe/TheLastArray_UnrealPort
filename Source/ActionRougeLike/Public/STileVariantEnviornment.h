// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STileVariantEnviornment.generated.h"

class ASTile;
class ASTileWall;
class USFTileVariantDefinitionData;
class ASTileDoor;

/// <summary>
/// Ideally this will sit on top of the tiles it corresponds to
/// </summary>
UCLASS()
class ACTIONROUGELIKE_API ASTileVariantEnviornment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTileVariantEnviornment();

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Population")
	TArray <UStaticMeshComponent*> PickupPlacements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Population")
	TArray <UStaticMeshComponent*> EnemyPlacements;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Population")
	TArray <UStaticMeshComponent*> ObjectivePlacements;


	UPROPERTY(EditAnywhere, Category = "Tile")
	TArray <ASTile*> VariantEncompassingTiles;

	/// <summary>
	/// Arrays of walls for each side
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Tile")
	TArray<ASTileWall*> UpWalls;
	UPROPERTY(EditAnywhere, Category = "Tile")
	TArray<ASTileWall*> DownWalls;
	UPROPERTY(EditAnywhere, Category = "Tile")
	TArray<ASTileWall*> LeftWalls;
	UPROPERTY(EditAnywhere, Category = "Tile")
	TArray<ASTileWall*> RightWalls;


	//Fully combined walls (Still WIP TBD if this is useful) - could make these privatre and have getters and setters
	UPROPERTY(EditAnywhere, Category = "Walls")
	UStaticMeshComponent* SMUpWallWhole;

	UPROPERTY(EditAnywhere, Category = "Walls")
	UStaticMeshComponent* SMDownWallWhole;

	UPROPERTY(EditAnywhere, Category = "Walls")
	UStaticMeshComponent* SMLeftWallWhole;

	UPROPERTY(EditAnywhere, Category = "Walls")
	UStaticMeshComponent* SMRightWallWhole;

	//Builds out array of doors that we run though after all pathing is figured out (so only a one time door removal)
	UPROPERTY(EditAnywhere, Category = "Tile")
	TArray<ASTileDoor*> DoorsToRemove;

	//definition data for Tile Variant (UDataAsset)
	UPROPERTY(EditAnywhere, Category = "Tile")
	USFTileVariantDefinitionData* TileVariDefinition;

	//this is manually placed in bp to be the starting tile we spawn at (not spawning at the center of this bp)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FTransform PresentSpawnPointTransform;

	//Due to spacing of tiles and door wys and gaps between tiles, we need to account for walls
	//side by side to not have any gaps. hence why we have these patches. Goal is to merge the static mesh of 
	//these guys with the walls themselves to create one coherent wall for each tile side.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallSetup")
	TArray <UStaticMeshComponent*> LeftPatchSMArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallSetup")
	TArray <UStaticMeshComponent*> RightPatchSMArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallSetup")
	TArray <UStaticMeshComponent*> UpPatchSMArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WallSetup")
	TArray <UStaticMeshComponent*> DownPatchSMArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
