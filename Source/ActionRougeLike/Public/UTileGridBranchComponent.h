// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STileVariantEnviornment.h"
#include "UTileGridBranchComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBranchFillGenerated);

class ASTileManager;
class ALocalLevel;
class ASTile;

/// <summary>
/// Sets up grid path addition includes setup for:
/// 
/// - Random rooms and branches
/// - single rooms
/// - secret room (if able)
/// - final setup on doors
/// - de-active inactive rooms
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API UTileGridBranchComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileGridBranchComponent();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void GameMapAdditionalSetup();

	UFUNCTION(Category = "Door Setup")
	void SetupDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect, ASTileDoor* door);

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* SecretRoom;

	//don't want things to be as statically coupled in manager so split things out with modular classes for each function of the map creation and setup
	UPROPERTY(BlueprintAssignable)

	FOnBranchFillGenerated OnGridAdditionalSetupCompletedEvent; //event to notify when path is created

protected:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void RandomRoomsAndBranchesAdditions();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void AddSingleRooms();

	UFUNCTION(BlueprintCallable, Category = "Abnormal Tile Placements")
	void GridScanForCustomTileSizedVariants();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	TArray <ASTile*> ReshuffleTiles(TArray <ASTile*> ar);

	UFUNCTION(BlueprintCallable, Category = "Abnormal Tile Placements")
	bool VariantCandidateAnalysis(ASTile* CurrentTile, USFTileVariantDefinitionData* CurrentVariant);

	UFUNCTION(Category = "Abnormal Tile Placements")
	bool PlugTile(FVariantOffsetTransforms_Rotates transformRotated, USFTileVariantDefinitionData* currentVariant, ASTile* CurrentTile, TArray <ASTile*>& EncompassingTilesBuild, FTileVariantSetup_PlugTileSaveInfo& transVariantPlugInfo);

	UFUNCTION(Category = "Abnormal Tile Placements")
	bool AddDoorsAndWalls(ASTile* CurrentTile, TArray<ASTileDoor*>& DoorsArray, TArray<ASTileWall*>& WallArray, TArray<FIntPointPair> SidesToCheck);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CreateSecretRoom();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void SpawnEndRoom();

	UFUNCTION(BlueprintCallable, Category = "Door Setup")
	void FinalDoorSetupDoors();

	UFUNCTION(BlueprintCallable, Category = "Door Setup")
	void DeactiveInactiveRooms();

	UFUNCTION(BlueprintCallable, Category = "Wall Setup")
	void MergeWallsForVariantTiles();

	UFUNCTION(BlueprintCallable, Category = "Wall Setup")
	UStaticMeshComponent* MergeWall(TArray<UStaticMeshComponent*> StaticMeshArrayToMerge);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CheckBranchTile(ASTile* TileToAdd, TArray<ASTile*>& CurrentPath, int Length, int prevDirection);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	int CheckPathSide(ASTile* TileToAdd);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void SingleRoomsDoorSetup(ASTile* CurrentTile);

	UFUNCTION(Category = "Door Setup")
	void SpawnDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void ConnectDoorBranch(ASTile* TileToAdd, int prevDirection);

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	//single rooms
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int FillerRooms = 0;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* choosen;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray <ASTileVariantEnviornment*> SpawnedVariants;
	

	// ---------------------------------
	// -- Cached references to external component pointers / variables --
	// ---------------------------------

	//needs reference to STileManager for accessing Grid and other properties
	UPROPERTY(EditAnywhere)
	ASTileManager* TileManagerRef;

	UPROPERTY(EditAnywhere)
	ALocalLevel* LocalLevelRef;

	//doesn't need to be pointer cause its value never changes on start
	UPROPERTY(EditAnywhere)
	bool DebugPrintsRef;

	//doesn't need to be pointer cause its value never changes on start
	UPROPERTY(EditAnywhere)
	bool DoorsActiveRef;

public:	
	
		
};
