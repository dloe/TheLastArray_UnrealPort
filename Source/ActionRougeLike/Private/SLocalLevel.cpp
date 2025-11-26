// Fill out your copyright notice in the Description page of Project Settings.

#include "SLocalLevel.h"
#include "LevelVariantPresetsData.h"
#include "SFTileVariantDefinitionData.h"
#include "Enviornment/TileGeneration/STile.h"

// Sets default values
ALocalLevel::ALocalLevel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALocalLevel::BeginPlay()
{
	Super::BeginPlay();

	SeedSetup();

	//get level tier / seed from save


	//based on what tier we are, use that levelvariantpresets (the tiles)
	AssignVariantDefData();

	//assign weight tables for items from data object

	//assign weight tables and enemies from data objects

	//assign weight tables and possible objectives from data objects


}

/// <summary>
/// Assign local variant references based on what lvl, this get assigned to data objects that are used
/// in the tile generation
/// </summary>
void ALocalLevel::AssignVariantDefData()
{
	//assign data for variants (using lvl 1 for now every time)
	//TODO: use other levels based on level choosen (using lvl 2 for secret room additions for now)


	//have to cast at int to use as indexing
	int32 CurrentLevelIndex = static_cast<int32>(CurrentLevelTier);
	OnexOneEnvVariants_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].OnexOneEnvVariants;
	TwoxOneEnvVariants_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].TwoxOneEnvVariants;
	TwoxTwoEnvVariants_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].TwoxTwoEnvVariants;
	ThreexOneEnvVariants_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].ThreexOneEnvVariants;
	ThreexTwoEnvVariants_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].ThreexTwoEnvVariants;
	FourxTwoEnvVariants_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].FourxTwoEnvVariants;
	FourxThreeEnvVariants_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].FourxThreeEnvVariants;
	FourxFourEnvVariants_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].FourxFourEnvVariants;
	PickupSpawnLevelThreshold_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].PickupSpawnLevelThreshold;
	EnemySpawnLevelThreshold_local = LevelVariantsPresets->LevelPresetInfo[CurrentLevelIndex + 1].EnemySpawnLevelThreshold;


	//TODO: need to check that the data objects getting assigned different lvl variant don't mess anything up
	OnexOneDefData->TileVariantEnviornmentsLocal = OnexOneEnvVariants_local;
	TwoxOneDefData->TileVariantEnviornmentsLocal = TwoxOneEnvVariants_local;
	TwoxTwoDefData->TileVariantEnviornmentsLocal = TwoxTwoEnvVariants_local;
	ThreexOneDefData->TileVariantEnviornmentsLocal = ThreexOneEnvVariants_local;
	ThreexTwoDefData->TileVariantEnviornmentsLocal = ThreexTwoEnvVariants_local;
	FourxTwoDefData->TileVariantEnviornmentsLocal = FourxTwoEnvVariants_local;
	FourxThreeDefData->TileVariantEnviornmentsLocal = FourxThreeEnvVariants_local;
	FourxFourDefData->TileVariantEnviornmentsLocal = FourxFourEnvVariants_local;
}

// Called every frame
void ALocalLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TSubclassOf<ASTile> ALocalLevel::GetPresetStartingTile()
{
	return PresetStartingTile;
}

void ALocalLevel::SeedSetup()
{
	if (GameSeed == 0)
	{
		if (bDebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Setting Up Seed..."));

		GameStream.Initialize("GameSeed");
		GameStream.GenerateNewSeed();
	}
	else {
		if (bDebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Using Supplied Seed..."));
		GameStream.Initialize(GameSeed);

	}

	if (bDebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Seed: %d"), GameStream.GetCurrentSeed());
}

/// <summary>
/// - list of objectives
/// - remove previous obj from list
/// - pick randomly from updated list
/// 
/// Each objective will have a weight to it, as we go deeper in the game, the easier objectives will be less likely
/// - Lvl4 will be a boss lvl so that one is already decided
/// </summary>
void ALocalLevel::ChooseObjective()
{

}

