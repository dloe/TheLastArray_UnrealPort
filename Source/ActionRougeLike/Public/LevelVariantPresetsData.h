// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SLocalLevel.h"
//#include "STileVariantEnviornment.h"
#include "Engine/DataAsset.h"
#include "LevelVariantPresetsData.generated.h"

class ASTileVariantEnviornment;


//for now put in a struct, not sure how to organize each level data (and stats)
//each level has stats,
//TODO: maybe this system may need to change? what if we did a few levels for each zone instead of just one?
USTRUCT()
struct FLevelInfoStats
{
	GENERATED_BODY()

public:

	//this isnt needed or used, but i should have this to label the data
	UPROPERTY(EditAnywhere, Category = "Lvl Info Data")
	ELevelTier LevelTier;

	UPROPERTY(EditAnywhere, Category = "Lvl Info Data")
	float PickupSpawnLevelThreshold; //density of pickup spawns

	UPROPERTY(EditAnywhere, Category = "Lvl Info Data")
	float EnemySpawnLevelThreshold; //density of enemy spawns

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> OnexOneEnvVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxOneEnvVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxTwoEnvVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexOneEnvVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexTwoEnvVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxTwoEnvVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxThreeEnvVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxFourEnvVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASStartingSpawnTile>> StartingTileVariants;

	UPROPERTY(EditAnywhere, Category = "Lvl Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> SecretVariants;
};

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ULevelVariantPresetsData : public UDataAsset
{
	GENERATED_BODY()
	
	public:

	UPROPERTY(EditAnywhere, Category = "Lvl Setup")
	TArray<FLevelInfoStats> LevelPresetInfo;

	


};

