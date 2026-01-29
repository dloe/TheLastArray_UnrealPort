// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SLocalLevel.h"
#include "Engine/DataTable.h"
#include "SFEnemyDataDefinition.generated.h"

/// <summary>
/// Struct for keeping tract of enemy data
/// datatable row for spawning monsters in gamemode
/// 
/// TODO: An array for variants for each level
/// </summary>
USTRUCT(BlueprintType)
struct FEnemySpawnInfo : public FTableRowBase {
	GENERATED_BODY()

	public:
		FEnemySpawnInfo() {
			Weight = 1.0f;
			SpawnCost = 5.0f;
			KillReward = 20.0f;
		}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	TSubclassOf<AActor> EnemySubclass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId; //USMonsterData* MonsterData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	int SpawnCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	FString EnemyName;

	//amount of credits awarded to killer of this unit
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;

};

/// <summary>
/// Tiers for each enemy
/// 
/// This just tells us who can we spawn in which level. there rest of the enemy specific data should be found 
/// via the FEnemySpawnInfo struct (in the data table)
/// </summary>
USTRUCT(BlueprintType)
struct FLevelTiersEnemyInfo {
	GENERATED_BODY()

	public:
		FLevelTiersEnemyInfo() {

		}

	UPROPERTY(EditAnywhere, Category = "Enemy Tier")
	ELevelTier Tier;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	TArray<FPrimaryAssetId> EnemyPoolID;
};

/**
 * TODO: Populate in editor, verify the enemy spawn BP is right component
 */
UCLASS()
class ACTIONROUGELIKE_API USFEnemyDataDefinition : public UDataAsset
{
	GENERATED_BODY()

	public:
	
	/// <summary>
    /// TODO: Build this out to include variants, squad building for spawning collection of enemies, etc
    /// </summary>
	UPROPERTY(EditAnywhere)
	TArray<FLevelTiersEnemyInfo> TieredData;

	//all available monsters, the data and contents (compared to tiered data which is who spawns in what lvl)
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MonsterTable;
};
