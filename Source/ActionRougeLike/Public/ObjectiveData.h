// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SLocalLevel.h"
#include "Engine/DataTable.h"
#include "Enviornment/PickupAssetData.h"
#include "ObjectiveData.generated.h"

USTRUCT()
struct FObjectiveAsset : public FTableRowBase
{
	GENERATED_BODY()

public:

	FObjectiveAsset() {
		ItemWeight = 1.0f;

	}
	
	//TODO: can still use weight in each tier but unsure if i want that much nuance
	UPROPERTY(EditAnywhere, Category = "Obj Info")
	int ItemWeight; 

	UPROPERTY(EditAnywhere, Category = "Obj Info")
	FString ObjectiveName;

	//levels can spawn in
	UPROPERTY(EditAnywhere, Category = "Obj Info")
	AActor* ObjectiveInteractable;

	UPROPERTY(EditAnywhere, Category = "Obj Info")
	FString ObjectiveDescription;

	//icon?

	//

};

USTRUCT(BlueprintType)
struct FLevelTiersObjectiveInfo {
	GENERATED_BODY()

	public:
		FLevelTiersObjectiveInfo() {

		}

	UPROPERTY(EditAnywhere, Category = "Enemy Tier")
	ELevelTier LevelTier;

	//each tier has its own list of tiers (higher the level, more common the drops?)
	UPROPERTY(EditAnywhere, Category = "Enemy Info")
	TArray<FTierForLevel> TierWeightsPerLevel;

	
};

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UObjectiveData : public UDataAsset
{
	GENERATED_BODY()

	public:

	//each value is added to a local array in ULevelAsset that we then use those weights to randomly select
	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FLevelTiersObjectiveInfo> ItemPickupTable;

	//all available objectives for all levels, the data and contents (compared to tiered data which is who spawns in what lvl)
	UPROPERTY(EditDefaultsOnly, Category = "Items")
	UDataTable* ObjectiveTable;
};
