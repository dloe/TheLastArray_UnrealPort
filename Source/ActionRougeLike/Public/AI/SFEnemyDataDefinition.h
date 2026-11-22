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


USTRUCT(BlueprintType)
struct FEnemyBehaviorData
{
	GENERATED_BODY()

	public:
		FEnemyBehaviorData() {
			CombatPos_OuterRadius = 0.0f;
			CombatPos_InnterRadius = 0.0f;
			CombatPos_DistanceToPlayerMin = 0.0f;
			InvestigationDuration = 0.0f;
			CombatRelocationCooldown = 0.0f;
			MinRelocationDistance = 0.0f;
		}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Behavior")
	float CombatPos_OuterRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Behavior")
	float CombatPos_InnterRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Behavior")
	float CombatPos_DistanceToPlayerMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	float InvestigationDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Behavior")
	float CombatRelocationCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Behavior")
	float MinRelocationDistance;

};

/// <summary>
/// Struct for keeping tract of enemy data
/// datatable row for spawning monsters in gamemode
/// - Spawn Info
/// - AI Behavior Stats
/// 
/// TODO: An array for variants for each level
/// </summary>
USTRUCT(BlueprintType)
struct FEnemyInfo : public FTableRowBase {
	GENERATED_BODY()

	public:
		FEnemyInfo() {
			Weight = 1.0f;
			SpawnCost = 5.0f;
			KillReward = 20.0f;
		}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Stats")
	TSubclassOf<AActor> EnemySubclass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId; //USMonsterData* MonsterData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Stats")
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Stats")
	int SpawnCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Stats")
	FString EnemyName;

	//amount of credits awarded to killer of this unit
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
	FEnemyBehaviorData EnemyBehaviorData;

	/// <summary>
	/// Validate values to guarantee designer doesn't forget anything again (me... im the designer)
	/// </summary>
	/// <param name="ErrorStr"></param>
	/// <returns></returns>
	bool Validate(TArray<FString>& OutErrors) const 
	{
		bool isValid = true;

		/*if (!EnemySubclass)
		{
			OutErrors.Add(TEXT("EnemySubclass Invalid"));
			isValid = false;
		}
		if (Weight < 0)
		{
			OutErrors.Add(TEXT("Weight Invalid. Less than 0."));
			isValid = false;
		}*/
		/*if (EnemyName.IsEmpty())
		{
			OutErrors.Add(TEXT("EnemyName empty."));
			isValid = false;
		}
		if (KillReward < 0)
		{
			OutErrors.Add(TEXT("KillReward Invalid. Less than 0."));
			isValid = false;
		}*/
		if (EnemyBehaviorData.CombatPos_OuterRadius < EnemyBehaviorData.CombatPos_InnterRadius)
		{
			OutErrors.Add(TEXT("BehaviorData - Combat Radius Invalid. Please check both."));
			isValid = false;
		}
		if (EnemyBehaviorData.CombatPos_DistanceToPlayerMin <= 0)
		{
			OutErrors.Add(TEXT("BehaviorData - CombatPos_DistanceToPlayer Invalid. Please check."));
			isValid = false;
		}
		if (EnemyBehaviorData.InvestigationDuration <= 0)
		{
			OutErrors.Add(TEXT("BehaviorData - InvestigationDuration Invalid. Please check."));
			isValid = false;
		}
		if (EnemyBehaviorData.CombatRelocationCooldown <= 0)
		{
			OutErrors.Add(TEXT("BehaviorData - CombatRelocationCooldown Invalid. Please check."));
			isValid = false;
		}
		if (EnemyBehaviorData.MinRelocationDistance <= 0)
		{
			OutErrors.Add(TEXT("BehaviorData - CombatRelocationCooldown Invalid. Please check."));
			isValid = false;
		}

		return isValid;
	}
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
			Tier = ELevelTier::ELevel_Null;
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
