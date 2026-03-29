// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Pickup/SPickupBase.h"
#include "Engine/DataTable.h"
#include "SLocalLevel.h"
#include "PickupAssetData.generated.h"

UENUM(BlueprintType)
	enum class ESpawnTiers : uint8 {
		ECommon UMETA(DisplayName = "Common"),          //58%
		EUncommon UMETA(DisplayName = "Uncommon"),      //25%
		ERare  UMETA(DisplayName = "Rare"),             //10%
		EEpic     UMETA(DisplayName = "Ebic"),          //5%
		ELegendary UMETA(DisplayName = "Legendary"),    //2%
		EObjective UMETA(DisplayName = "Objective")     //guaranteed for certain levels (depends on objective)
	};

USTRUCT()
struct FItemPickupAsset : public FTableRowBase
{
	GENERATED_BODY()

public:

	FItemPickupAsset() {
		ItemWeight = 1.0f;
		ItemTier = ESpawnTiers::ECommon;
		//SpawnCost = 5.0f;
		//KillReward = 20.0f;
	}
	
	//TODO: can still use weight in each tier but unsure if i want that much nuance
	UPROPERTY(EditAnywhere, Category = "Item Info")
	int ItemWeight; 

	UPROPERTY(EditAnywhere, Category = "Item Info")
	FString ItemName;

	UPROPERTY(EditAnywhere, Category = "Item Info")
	ESpawnTiers ItemTier;

	//physical asset to spawn
	UPROPERTY(EditAnywhere, Category = "Item Info")
	TSubclassOf<ASPickupBase> PickupPrefab;

	//might use ID instead of PickupPrefab
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId ItemId;
};

USTRUCT(BlueprintType)
struct FTierForLevel {
	GENERATED_BODY()

public:
	FTierForLevel() {
		CurrentTier = ESpawnTiers::EUncommon;
	}

	UPROPERTY(EditAnywhere, Category = "Item Info")
	ESpawnTiers CurrentTier;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	TArray<FPrimaryAssetId> ItemPoolID;
};

/// <summary>
/// Tiers for each enemy
/// 
/// This just tells us who can we spawn in which level. there rest of the enemy specific data should be found 
/// via the FEnemySpawnInfo struct (in the data table)
/// </summary>
USTRUCT(BlueprintType)
struct FLevelTiersItemInfo {
	GENERATED_BODY()

	public:
		FLevelTiersItemInfo() {
			LevelTier = ELevelTier::ELevel_1;
		}

	UPROPERTY(EditAnywhere, Category = "Enemy Tier")
	ELevelTier LevelTier;

	//each tier has its own list of tiers (higher the level, more common the drops?)
	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	TArray<FTierForLevel> TierWeightsPerLevel;

	
};

/**
 * Going to use rarity tiers for item spawning
 * 
 * Each tier has a percentage attached, then the items will be in each of its corresponding buckets and spawned randomly
 * 
 * Common
 * Uncommon
 * Rare
 * Epic
 * Legendary? might not use idk yet
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UPickupAssetData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//each value is added to a local array in ULevelAsset that we then use those weights to randomly select
	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FLevelTiersItemInfo> ItemPickupTable;

	//all available items for level, the data and contents (compared to tiered data which is who spawns in what lvl)
	UPROPERTY(EditDefaultsOnly, Category = "Items")
	UDataTable* ItemTable;
};
