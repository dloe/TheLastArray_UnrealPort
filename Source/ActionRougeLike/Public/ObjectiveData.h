// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SLocalLevel.h"
#include "Engine/DataTable.h"
#include "Enviornment/PickupAssetData.h"
#include "Objectives/SBaseObjective.h"
#include "ObjectiveData.generated.h"

//class SBaseObjective;

/// <summary>
/// This should store the objective class to spawn
/// 
/// This should also store any spawning info, if i wanted a basic weight system
/// </summary>
USTRUCT(BlueprintType)
struct FLevelTiersObjectiveInfo {
	GENERATED_BODY()

	public:
		FLevelTiersObjectiveInfo() {

		}

	UPROPERTY(EditAnywhere, Category = "Objecive Info")
	ELevelTier LevelTier;

	//All possible objectives to spawn TODO: Swap out class
	UPROPERTY(EditAnywhere, Category = "Objecive Info")
	TArray<TSubclassOf<USBaseObjective>> PossibleObjectives;
};

/**
 * TODO: Should this be tiers of rarity for objectives? I feel like i would want them all to be of equal 
 * spawning tbh... may update later.
 */
UCLASS()
class ACTIONROUGELIKE_API UObjectiveData : public UDataAsset
{
	GENERATED_BODY()

	public:

	//each objective should be the same chance of spawning
	//each value is added to a local array in ULevelAsset that we then use those weights to randomly select
	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FLevelTiersObjectiveInfo> ObjectivesTable;


	//instead of following the similar structure of items and enemies where its mainly stat differences 
	//with AssetIds, spawn tier this will be an array of objective child classes i randomly pick (unless boss room?)
	//UPROPERTY(EditDefaultsOnly, Category = "Items")
	//UDataTable* ObjectiveTable;
};
