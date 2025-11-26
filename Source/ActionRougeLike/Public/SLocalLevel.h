// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STileVariantEnviornment.h"
#include "SLocalLevel.generated.h"

UENUM(BlueprintType)
	enum class ELevelTier : uint8 {
		ELevel_Null UMETA(DisplayName = "NullLevelTier"),     //1
		ELevel_1 UMETA(DisplayName = "Level1"),               //2
		ELevel_2  UMETA(DisplayName = "Level2"),              //3
		ELevel_3     UMETA(DisplayName = "Level3"),           //4
		ELevel_4 UMETA(DisplayName = "Level4"),               //5
		ELevel_Train UMETA(DisplayName = "TrainLevel")        //6 - training level tutorial
	};

	class ULevelVariantPresetsData;
	class STile;

UCLASS()
class ACTIONROUGELIKE_API ALocalLevel : public AActor
{
	/// <summary>
	/// Local Level
	/// Dylan Loe
	/// 
	/// 
	/// Notes:
	/// - Keeps track of local level info and setup
	/// - sets one of the objective types 
	/// - sets up what type of tile assets we use
	///		- IE StartingTile preset
	///		- pulls and assigns level assets from Scriptable objects
	///		- pulls and sets types or variants of level objectives
	/// 
	/// Potentially could include:
	/// - 
	/// 
	/// </summary>
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALocalLevel();

	// ---------------------------------
	// ------- Public Variables --------
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Game Setup Generation")
	uint32 GameSeed = 0; 

	//managing random numbers and seeds
	UPROPERTY(EditAnywhere, Category = "Game Setup Generation")
	FRandomStream GameStream;

	UPROPERTY(EditAnywhere, Category = "Game Setup Generation")
	bool bDebugPrints = false;

	//this is assigned via LocalLevel
	UPROPERTY(EditAnywhere, Category = "Tile Configuration")
	TSubclassOf<ASTile> PresetStartingTile;

	UPROPERTY(EditAnywhere, Category = "Tile Configuration")
	TSubclassOf<ASTile> PresetSecretRoomTile;

	UPROPERTY(EditAnywhere, Category = "Tile Stats")
	ELevelTier CurrentLevelTier;

	//reference level asset data and presets

	//each level should have some type of data object that holds this info, then will assign this this guy
	UPROPERTY(EditAnywhere, Category = "Tile Stats")
	float LevelItemPlacementThresholds; 

	//player data?

	//objective

	//reference to player

	//presets for each level tier (TODO: move this to new location for storage)

	//reference to data object that contains all the possible variants for each level
	//at start based on what level, we assign those variants to their corresponding TileVariantDefData data object
	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> OnexOneEnvVariants_local;

	//UPROPERTY(EditAnywhere, Category = "Tile Variants")
	//TArray<ASTileVariantEnviornment> OnexOneEnvVariants_local_test;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxOneEnvVariants_local;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxTwoEnvVariants_local;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexOneEnvVariants_local;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexTwoEnvVariants_local;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxTwoEnvVariants_local;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>>FourxThreeEnvVariants_local;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxFourEnvVariants_local;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	float PickupSpawnLevelThreshold_local;

	UPROPERTY(EditAnywhere, Category = "Tile Variants")
	float EnemySpawnLevelThreshold_local;

	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	ULevelVariantPresetsData* LevelVariantsPresets;


	//each variant data structure
	//(contains the array of potential variants we assign above)
	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	USFTileVariantDefinitionData* OnexOneDefData;

	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	USFTileVariantDefinitionData* TwoxOneDefData;

	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	USFTileVariantDefinitionData* TwoxTwoDefData;

	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	USFTileVariantDefinitionData* ThreexOneDefData;

	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	USFTileVariantDefinitionData* ThreexTwoDefData;

	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	USFTileVariantDefinitionData* FourxTwoDefData;

	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	USFTileVariantDefinitionData* FourxThreeDefData;

	UPROPERTY(EditAnywhere, Category = "Local Level Data")
	USFTileVariantDefinitionData* FourxFourDefData;

	// ---------------------------------
	// -------- Public Functions -------
	// ---------------------------------
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TSubclassOf<ASTile> GetPresetStartingTile();

	

	UFUNCTION(BlueprintCallable, Category = "Level Objective")
	void ChooseObjective();

protected:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "Level setup")
	void SeedSetup();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UFUNCTION(BlueprintCallable, Category = "Level Objective")
	//TArray<TSubclassOf<ASTileVariantEnviornment>> SetVariantEnvVars(TArray<TSubclassOf<ASTileVariantEnviornment>> VariantsEnvArray);

	UFUNCTION(BlueprintCallable, Category = "Level Objective")
	void AssignVariantDefData();



	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------


};


