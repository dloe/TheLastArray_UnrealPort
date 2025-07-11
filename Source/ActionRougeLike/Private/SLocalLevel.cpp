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

	//get level tier from save

	//assign data for variants
	OnexOneEnvVariants_local = LevelVariantsPresets->OnexOneEnvVariants_lvl1;
	TwoxOneEnvVariants_local = LevelVariantsPresets->TwoxOneEnvVariants_lvl1;
	TwoxTwoEnvVariants_local = LevelVariantsPresets->TwoxTwoEnvVariants_lvl1;
	ThreexOneEnvVariants_local = LevelVariantsPresets->ThreexOneEnvVariants_lvl1;
	ThreexTwoEnvVariants_local = LevelVariantsPresets->ThreexTwoEnvVariants_lvl1;
	FourxTwoEnvVariants_local = LevelVariantsPresets->FourxTwoEnvVariants_lvl1;
	FourxThreeEnvVariants_local = LevelVariantsPresets->FourxThreeEnvVariants_lvl1;
	FourxFourEnvVariants_local = LevelVariantsPresets->FourxFourEnvVariants_lvl1;

	OnexOneDefData->TileVariantEnviornments = OnexOneEnvVariants_local;
	TwoxOneDefData->TileVariantEnviornments = TwoxOneEnvVariants_local;
	TwoxTwoDefData->TileVariantEnviornments = TwoxTwoEnvVariants_local;
	ThreexOneDefData->TileVariantEnviornments = ThreexOneEnvVariants_local;
	ThreexTwoDefData->TileVariantEnviornments = ThreexTwoEnvVariants_local;
	FourxTwoDefData->TileVariantEnviornments = FourxTwoEnvVariants_local;
	FourxThreeDefData->TileVariantEnviornments = FourxThreeEnvVariants_local;
	FourxFourDefData->TileVariantEnviornments = FourxFourEnvVariants_local;

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

/// <summary>
/// - list of objectives
/// - remove previous obj from list
/// - pick randomly from updated list
/// </summary>
void ALocalLevel::ChooseObjective()
{

}

