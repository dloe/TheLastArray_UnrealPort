// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enviornment/TileGeneration/STileDoor.h"
#include "STile.generated.h"

UENUM(BlueprintType)
	enum class ETileStatus : uint8 {
		ETile_NULLROOM UMETA(DisplayName = "NullRoom"),
		ETile_PATH  UMETA(DisplayName = "Path"),
		ETile_ROOM     UMETA(DisplayName = "Room"),
		ETile_STARTINGROOM UMETA(DisplayName = "StartingRoom"),
		ETile_SECRETROOM UMETA(DisplayName = "SecretRoom"),
		ETile_BOSSROOM UMETA(DisplayName = "BossRoom"),
	};

	UENUM(BlueprintType)
	enum class ETileSide : uint8 {
		ETile_Up UMETA(DisplayName = "Up"),
		ETile_Down  UMETA(DisplayName = "Down"),
		ETile_Left     UMETA(DisplayName = "Left"),
		ETile_Right UMETA(DisplayName = "Right"),
	};

UCLASS()
class ACTIONROUGELIKE_API ASTile : public AActor
{
	/// <summary>
	/// Tile
	/// Dylan Loe
	/// 
	/// Notes:
	/// - Core building block of tile map
	/// - will contain references to doors connecting each other
	/// 
	/// </summary>
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Tile Setup")
	void SetUpDoorTransforms();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* DoorsRoot;

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Walls")
	void ActivateWalls();

#pragma region Tile Stats
	UPROPERTY(EditDefaultsOnly, Category = "Tile Stats")
	float TileLength;

	//what our index is on the array
	UPROPERTY(EditAnywhere, Category = "Tile Stats")
	int32 XIndex;
	UPROPERTY(EditAnywhere, Category = "Tile Stats")
	int32 ZIndex;

	// Contains a reference to the present used for overall Tile layout
	UPROPERTY(EditAnywhere, Category = "Tile Stats")
	AActor* PresetTile;
	

	UPROPERTY(EditAnywhere, Category = "Tile Stats")
	ETileStatus TileStatus;

	UFUNCTION(Category = "Tile Stats")
	bool IsBossTile() const;

	UFUNCTION(Category = "Tile Stats")
	bool IsStartingTile() const;

#pragma endregion

#pragma region Tile Doors

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Doors")
	void ActivateDoorToPath();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Doors")
	void ActivateDoorsBranch();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Doors")
	void ActivateDoorsRandom();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Doors")
	void SyncDoors();

#pragma endregion

#pragma region Tile Path

	UPROPERTY(EditAnywhere, Category = "Tile Components - Path")
	ASTile* PreviousTile;

	UPROPERTY(EditAnywhere, Category = "Tile Components - Path")
	bool CheckForPath = false;

	UPROPERTY(EditAnywhere, Category = "Tile Components - Path")
	bool PartOfPath = false;

	UPROPERTY(EditAnywhere, Category = "Tile Components - Path")
	int PathNumber = -1;

	UPROPERTY(EditAnywhere, Category = "Tile Components - Path")
	bool EndOfPath = false;

	UPROPERTY(EditAnywhere, Category = "Tile Components - Path")
	bool EndOfBranchPath = false;

	UPROPERTY(EditAnywhere, Category = "Tile Components - Path")
	bool ConnectedToPath = false;

	UPROPERTY(EditAnywhere, Category = "Tile Components - Path")
	FString TileDescription = "";

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Path Debug")
	void ShadeNull();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Path Debug")
	void ShadePath();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Path Debug")
	void ShadeActiveRoom();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Path Debug")
	void ShadeBossRoom();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Path Debug")
	void ShadeStartingRoom();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Path Debug")
	void ShadeSecretRoom();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Path Debug")
	void ShadeTestRoom();

	UFUNCTION(BlueprintCallable, Category = "Tile Components - Path Debug")
	void ShadeEndRoom();

#pragma endregion

#pragma region Tile Neighbors
	//Neighbors
	UPROPERTY(EditAnywhere, Category = "Tile Components - Neighbors")
	ASTile* UpNeighbor; //index above us, aka greater 
	UPROPERTY(EditAnywhere, Category = "Tile Components - Neighbors")
	ASTile* DownNeighbor;
	UPROPERTY(EditAnywhere, Category = "Tile Components - Neighbors")
	ASTile* LeftNeighbor;
	UPROPERTY(EditAnywhere, Category = "Tile Components - Neighbors")
	ASTile* RightNeighbor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Components - Neighbors")
	ASTileDoor* UpDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Tile Components - Neighbors")
	ASTileDoor* DownDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Components - Neighbors")
	ASTileDoor* LeftDoor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Components - Neighbors")
	ASTileDoor* RightDoor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Components - Neighbors")
	FTransform UpDoorSpawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Components - Neighbors")
	FTransform DownDoorSpawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Components - Neighbors")
	FTransform LeftDoorSpawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Components - Neighbors")
	FTransform RightDoorSpawnPoint;

	UFUNCTION(Category = "Tile Components - Neighbors")
	bool HasValidRightNeighbor();

	UFUNCTION(Category = "Tile Components - Neighbors")
	bool HasValidLeftNeighbor();

	UFUNCTION(Category = "Tile Components - Neighbors")
	bool HasValidDownNeighbor();

	UFUNCTION(Category = "Tile Components - Neighbors")
	bool HasValidUpNeighbor();

	//These are neighbors that are existing that we want to connect to
	UFUNCTION(Category = "Tile Components - Neighbors")
	bool HasConnectedRightNeighbor();

	UFUNCTION(Category = "Tile Components - Neighbors")
	bool HasConnectedLeftNeighbor();

	UFUNCTION(Category = "Tile Components - Neighbors")
	bool HasConnectedUpNeighbor();

	UFUNCTION(Category = "Tile Components - Neighbors")
	bool HasConnectedDownNeighbor();

	UFUNCTION(Category = "Tile Components - Doors")
	void TurnAllDoorsInactive();

	//connect tile door to path (so not a valid tile that could be placed but a tile that already exists there)
	UFUNCTION(Category = "Tile Components - Doors")
	void ConnectUpDoor();
	UFUNCTION(Category = "Tile Components - Doors")
	void ConnectDownDoor();
	UFUNCTION(Category = "Tile Components - Doors")
	void ConnectLeftDoor();
	UFUNCTION(Category = "Tile Components - Doors")
	void ConnectRightDoor();

	UFUNCTION(Category = "Tile Components - Doors")
	void ActivateUpDoor();

	UFUNCTION(Category = "Tile Components - Doors")
	void ActivateDownDoor();

	UFUNCTION(Category = "Tile Components - Doors")
	void ActivateRightDoor();

	UFUNCTION(Category = "Tile Components - Doors")
	void ActivateLeftDoor();

#pragma endregion

};
