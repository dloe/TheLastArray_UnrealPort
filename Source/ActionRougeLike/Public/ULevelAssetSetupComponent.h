// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enviornment/TileGeneration/STileManager.h"
#include "SLocalLevel.h"
#include "Enviornment/PickupAssetData.h"
#include "SFEnemyDataDefinition.h"
#include "STileVariantEnviornment.h"
#include "ULevelAssetSetupComponent.generated.h"


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

	UPROPERTY(EditAnywhere)
	ASTileManager* TileManagerRef;

	UPROPERTY(EditAnywhere)
	ALocalLevel* LocalLevel;

	UPROPERTY(EditAnywhere) //how many items have we placed?
	int PickupsPlaced;

	UPROPERTY(EditAnywhere) //how many items have we placed?
	int EnemiesPlaced;

	UPROPERTY(EditAnywhere)
	UPickupAssetData* ItemData;

	UPROPERTY(EditAnywhere)
	USFEnemyDataDefinition* EnemyData;

	UPROPERTY(EditAnywhere)
	bool ActivateDebugFloorPerlinNoise = false;

	UPROPERTY(EditAnywhere)
	AActor* DebugPerlinNoiseFloor;

	UPROPERTY(EditAnywhere)
	float MaxNoise = 0;

	UPROPERTY(EditAnywhere)
	float MinNoise = 0;

protected:

	// ---------------------------------
	// -- Internal state / variables  --
	// ---------------------------------

	//idea is that we add by item weight then choose randomly and the weights should be good
	//UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	//TArray<FItemPickupAsset> LevelItemDropWeightTable;

	UPROPERTY(EditAnywhere, Category = "Component References")
	UTileGridBranchComponent* GridBranchCompRef;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	TArray <ASTileVariantEnviornment*> SpawnedVariantsRef;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	FLevelTiersEnemyInfo LocalEnemyInfoData;

	UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	FLevelTiersItemInfo LocalItemInfoData;


	//UPROPERTY(EditAnywhere, Category = "Enemies")
	TArray <FEnemySpawnInfo*> CurrentLevelEnemyList;

	TArray <FLocalLevelItemSpawnTiers> CurrentLevelItemTierList;

	//same offset for all checks on asset spawning with Perlin noise
	UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	FVector2D seedOffset;


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
	void PlaceItemPickup(UStaticMeshComponent* PickupMarker, ASTileVariantEnviornment* AttachedTile);

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void PlaceEnemy(UStaticMeshComponent* PickupMarker, ASTileVariantEnviornment* AttachedTile);

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	ESpawnTiers GetTierOnPercent(float inputFloat);


	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateEnemies();

	//UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	FEnemySpawnInfo* GetWeightedRandomEnemy();

	UFUNCTION(BlueprintCallable)
	float GetNoiseVec(FVector inputCords);
	
	//For loading enemies
	void OnEnemyLoaded(FEnemySpawnInfo* EnemySpawnInfo, FVector SpawnLocation);

	void OnPickupLoaded(FItemPickupAsset* ItemSpawnInfo, FVector SpawnLocation, FRotator spawnRotation, ASTileVariantEnviornment* AttachedTile);

	UFUNCTION(BlueprintCallable)
	void SetUpDebugPerlinNoise();

	//debug see texture on floors 
	UFUNCTION(BlueprintCallable)
	UTexture2D* DebugCreatePerlinNoiseTexture(int32 size, FBoxSphereBounds Bounds);
};

