// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enviornment/TileGeneration/STile.h"
#include "SLocalLevel.h"
#include "Engine/World.h"
#include "STileManager.generated.h"

//forward declaring
class UTilePathSetupComp;

USTRUCT()
struct FMultiTileStruct
{
	GENERATED_BODY()

public:
	//each element of our array will contain this array of AActor*
	UPROPERTY()
	//TArray<TSubclassOf<ASTile>> TileColumn;
	TArray<ASTile*> TileColumn;

	//Constructor 
	FMultiTileStruct()
	{
		//UE_LOG(LogTemp, Log, TEXT("In FMultiTileStruct Constructor..."));
		TileColumn.Empty();
	}

};

USTRUCT()
struct FTileInfoStruct
{
	GENERATED_BODY()

public:

	UPROPERTY()
	ASTile* tile;
	UPROPERTY()
	TArray<int> neighborArray;


};

UCLASS()
class ACTIONROUGELIKE_API ASTileManager : public AActor
{
	/// <summary>
	/// Tile Manager Script
	/// Dylan Loe
	/// 
	/// Notes:
	/// - Main Manager for Map creation and Tiles, more monolithic in nature
	/// - Holds all linked tiles in 2d array
	/// - Runs the initial random tile generation implementation
	/// - Receives info for map size and criteria from Scriptable objects
	/// 
	/// 
	/// 2D arrays are difficult but we can try the struct approach
	/// 
	/// </summary>
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTileManager();

#pragma region Tile Generation

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	uint32 GameSeed = 0; 

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	bool DebugPrints = false;

	//attempt at managing random numbers and seeds
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FRandomStream GameStream;


	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TSubclassOf<ASTile> TileBase;
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TSubclassOf<ASTileDoor> TileDoor;
	

	UFUNCTION()
	int GetLevelHeight() { return LevelHeight; };
	

	UFUNCTION()
	int GetLevelWidth() { return LevelWidth; };

	//density of grid - percentage of non-main path tiles compared to total tiles while still not overcrowding
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	float gridDensity = 0.5;

	/// <summary>
	/// 
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	float BranchDensityFactor_DynamicMainPathLength();
	//float BranchDensityFactor = FMath::Clamp(0.1f + (LevelWidth * LevelHeight / 5000.0f), 0.05f, 0.3f);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	float BranchDensityFactor_DependencyOnMazeSize();

	UFUNCTION()
	ASTile* GetGridTile(int32 X, int32 Y);

	//UFUNCTION()
	//TArray <FMultiTileStruct> GetGrid() {return Grid2DArray; };
	//2D array to hold all tiles
	TArray <FMultiTileStruct*> Grid2DArray;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int totalGridTilesAvailable = -1;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ALocalLevel* MyLocalLevel;

	//are doors to be used in levels, some might not need them
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	bool DoorsActive = true;

	//root component
	UPROPERTY(VisibleAnywhere)
	USceneComponent* TilesRoot;

	//folder name for where tiles are placed when spawned
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FName TileSubFolderName;

	//folder name for where doors are placed when spawned
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FName DoorSubFolderName;

	//folder name for where doors are placed when spawned
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FName TileGenRootFolder;

	//starting tile reference - TO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* StartingTile;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* choosen;

	//starting tile reference - TO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* EndTile;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* SecretRoom;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*> PossibleStartingTiles;

	UFUNCTION()
	TArray<ASTile*> GetPossibleStartingTiles() const {return PossibleStartingTiles;};

	UFUNCTION()
	void SetPossibleStartingTiles(TArray<ASTile*> NewArray) { PossibleStartingTiles  = NewArray;};
	

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*> LevelPath;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*> BackTrackHistory;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTileDoor*> DoorArray;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int PathNumber = 0;

	//list of current active tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*>	AllActiveTiles;

	//list of possible branching tiles, they can be used off main path or branches - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*>	AvailableTiles;

	//single rooms
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int FillerRooms = 0;

	UPROPERTY(EditAnywhere, Category = "Start Room")
	bool DoorToStartRoom = true;
	
#pragma endregion

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


#pragma region Tile Generation
	

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int LevelHeight = 5;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int LevelWidth = 5;

	UPROPERTY()
    UTilePathSetupComp* TilePathComponent;


	//Spawned in from LocalLevel
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* PlayerSpawnPresentTile;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTile* PlayerStartingTileBase; //TODO: difference between this and StartingTile???

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<FTileInfoStruct> OutskirtTiles;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int FailsafeCount = 0;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FTransform doorTransform;

	UFUNCTION()
	void OnTilePathGeneration();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void TileMapSetup();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void SeedSetup();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void Create2DTileArray();

	UFUNCTION(Category = "ArrayCreation")
	void LinkTile(ASTile* ThisTile, FMultiTileStruct Col);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void ChooseStartEndRooms();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void GeneratePath();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	bool AddTileToPath(ASTile* TileToAdd);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CheckTile(ASTile* TileToAdd, TArray<ASTile*>& CurrentPath);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void ClearHistory();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void RandomRoomsAndBranchesAdditions();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void AddSingleRooms();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void SingleRoomsDoorSetup(ASTile* CurrentTile);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CreateSpawnRoom();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CreateSecretRoom();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CheckBranchTile(ASTile* TileToAdd, TArray<ASTile*>& CurrentPath, int Length, int prevDirection);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void ConnectDoorBranch(ASTile* TileToAdd, int prevDirection);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	int CheckPathSide(ASTile* TileToAdd);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void MakeAvailableTiles();

	UFUNCTION(BlueprintCallable, Category = "Door Setup")
	void FinalDoorSetupDoors();

	UFUNCTION(BlueprintCallable, Category = "Door Setup")
	void DeactiveInactiveRooms();

	UFUNCTION(Category = "Door Setup")
	void SpawnDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect, FTransform doorAdjustment);

	UFUNCTION(Category = "Door Setup")
	void SetupDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect, ASTileDoor* door);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	TArray <int> Reshuffle2(TArray <int> ar);

	UFUNCTION(BlueprintCallable, Category = "Tile Generation")
	float GetCurrentGridDensity();

	//For debug if we want to hard code a specific side to test, we set to 0 -3 otherwise it will get overridden
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int StartRoomSide = -1;
#pragma endregion


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
