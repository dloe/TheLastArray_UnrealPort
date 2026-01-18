// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enviornment/TileGeneration/STileManager.h"
#include "SLocalLevel.h"
#include "Enviornment/PickupAssetData.h"
#include "SFEnemyDataDefinition.h"
#include "STileVariantEnviornment.h"
#include "ObjectiveData.h"
#include "ULevelAssetSetupComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemiesSpawned);

/// <summary>
/// This is local structure that is built off the ItemData table and the UPickupAssetData TieredItem info we build 
/// and configure
/// </summary>
//USTRUCT(BlueprintType)
struct FLocalLevelItemSpawnTiers {
	//GENERATED_BODY()

public:
	FLocalLevelItemSpawnTiers() {

	}

	FLocalLevelItemSpawnTiers(ESpawnTiers tier) {
		CurrentTier = tier;
	}

	//UPROPERTY(EditAnywhere, Category = "Item Info")
	ESpawnTiers CurrentTier;

	//UPROPERTY(EditAnywhere, Category = "Enemy Info")
	TArray<FItemPickupAsset*> ItemPoolAsset;
};

//delegate for sub / pub system for removing pickups after spawn
DECLARE_MULTICAST_DELEGATE(FOnCleanupPickups);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API ULevelAssetSetupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULevelAssetSetupComponent();

	// ---------------------------------
	// -------- Public Functions -------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void PopulateGridAssets();


	//idea for pickup markers cleanup
	FOnCleanupPickups OnCleanupPickups;

	void CleanupAllItemPickups()
	{
		OnCleanupPickups.Broadcast();
	}

	// ---------------------------------
	// ------- Public Variables --------
	// ---------------------------------

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Set to non -1 to use objective for possible objectives FOR CURRENT LEVEL. Check ObjectiveData for stats for which numbers"))
	int OverrideObjectiveChoice = -1;

	UPROPERTY(EditAnywhere)
	ASTileManager* TileManagerRef;

	UPROPERTY(EditAnywhere)
	ALocalLevel* LocalLevel;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Populated on play.")) //how many items have we placed?
	int PickupsPlaced;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Populated on play.")) //how many items have we placed?
	int EnemiesPlaced;

	//combine the item, enemy and objective data into same data objective. ALSO if i was clever i would
	// combine these with the variance data i already have set up and sorted out. I could keep each levels 
	//everything organized in one big data object
	UPROPERTY(EditAnywhere)
	UPickupAssetData* ItemData;

	UPROPERTY(EditAnywhere)
	USFEnemyDataDefinition* EnemyData;

	UPROPERTY(EditAnywhere)
	UObjectiveData* ObjectiveData;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	bool ActivateDebugFloorPerlinNoise = false;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	AActor* DebugPerlinNoiseFloor;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play."))
	float MaxNoiseItems = 0;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play."))
	float MinNoiseItems = 0;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play."))
	float MaxNoiseEnemies = 0;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play."))
	float MinNoiseEnemies = 0;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta=(ToolTip= "1 is items, 0 is enemies. Must be set before play."))
	bool DebugTextureToggle;


	UPROPERTY(BlueprintAssignable)

	FOnEnemiesSpawned OnEnemySpawnCompletedEvent;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play."))
	TArray<AActor*> SpawnedEnemiesInLevel;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play."))
	TArray<AActor*> SpawnedPickupsInLevel;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play."))
	ASStartingSpawnTile* SpawnTileEnvRef;

protected:

	// ---------------------------------
	// -- Internal state / variables  --
	// ---------------------------------

	//idea is that we add by item weight then choose randomly and the weights should be good
	//UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	//TArray<FItemPickupAsset> LevelItemDropWeightTable;

	UPROPERTY(EditAnywhere, Category = "Component References")
	UTileGridBranchComponent* GridBranchCompRef;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play. Spawned Variants in level"))
	TArray <ASTileVariantEnviornment*> SpawnedVariantsRef;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play. Assigned. Enemy Data per level"))
	FLevelTiersEnemyInfo LocalEnemyInfoData;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play. Assigned item Data per level"))
	FLevelTiersItemInfo LocalItemInfoData;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play. Assigned objective spawn Data per level"))
	FLevelTiersObjectiveInfo LocalObjectiveInfoData;


	//UPROPERTY(EditAnywhere, Category = "Enemies")
	TArray <FEnemySpawnInfo*> CurrentLevelEnemyList;

	TArray <FLocalLevelItemSpawnTiers> CurrentLevelItemTierList;

	//same offset for all checks on asset spawning with Perlin noise
	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play. Randomized offset for item perlin noise map."))
	FVector2D seedOffset_Items;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population", meta = (ToolTip = "Populated on play. Randomized offset for enemy perlin noise map."))
	FVector2D seedOffset_Enemies;

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void SetupLevelAssetComponent();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void GridAnalysis();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateLevelKey();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateSecretRoom();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateObjectives();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateItems();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void PlaceItemPickup(UStaticMeshComponent* PickupMarker, ASTileVariantEnviornment* AttachedTile, int PlacementNum);

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void PlaceEnemy(UStaticMeshComponent* PickupMarker, ASTileVariantEnviornment* AttachedTile, int numberSpawned);

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	ESpawnTiers GetTierOnPercent(float inputFloat);

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateEnemies();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void SetupStartingTile();

	//UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	FEnemySpawnInfo* GetWeightedRandomEnemy();

	UFUNCTION(BlueprintCallable)
	float GetNoiseVec(FVector2D inputCords, float MinNoise, float MaxNoise);
	
	//For loading enemies
	void OnEnemyLoaded(FEnemySpawnInfo* EnemySpawnInfo, FVector SpawnLocation, int enemyNum);

	void OnPickupLoaded(FItemPickupAsset* ItemSpawnInfo, FVector SpawnLocation, FRotator spawnRotation, ASTileVariantEnviornment* AttachedTile, int itemNum);

	UFUNCTION(BlueprintCallable)
	void SetUpDebugPerlinNoise();

	template<typename T>
	TArray <T> ReshuffleArray(TArray <T> ar)
	{
		// Knuth shuffle algorithm :: courtesy of Wikipedia :)
		for (int t = 0; t < ar.Num(); t++)
		{
			int r = LocalLevel->GameStream.RandRange(t, ar.Num() - 1);
			ar.Swap(t, r);
		}
		return ar;
	};

	//debug see texture on floors 
	UFUNCTION(BlueprintCallable)
	UTexture2D* DebugCreatePerlinNoiseTexture(int32 size, FBoxSphereBounds Bounds);
};



