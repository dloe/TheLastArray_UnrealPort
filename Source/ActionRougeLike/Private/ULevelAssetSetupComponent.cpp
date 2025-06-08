// Fill out your copyright notice in the Description page of Project Settings.


#include "ULevelAssetSetupComponent.h"

// Sets default values for this component's properties
ULevelAssetSetupComponent::ULevelAssetSetupComponent()
{

}


// Called when the game starts
void ULevelAssetSetupComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

/// <summary>
/// Since we cant run much in BeginPlay due to TileManager setting everything up, that setup is now run here
/// </summary>
void ULevelAssetSetupComponent::SetupLevelAssetComponent()
{
	//set up the asset count array

	//set up the big asset count array

	//select which tier our level is


}

void ULevelAssetSetupComponent::GridAnalysis()
{

}

void ULevelAssetSetupComponent::ActivateLevelKey()
{

}

void ULevelAssetSetupComponent::ActivateSecretRoom()
{

}

void ULevelAssetSetupComponent::ActivateObjectives()
{

}

void ULevelAssetSetupComponent::ActivateItems()
{

}

void ULevelAssetSetupComponent::ActivateEnemies()
{

}

/// <summary>
/// Populate grid with assets, called from TileGeneration once it is done setting up
/// 
/// - Handles setup of for this component, setup for big tiles, level key, activating secret room, objective, items, enemies
/// </summary>
void ULevelAssetSetupComponent::PopulateGrid()
{






	//Finished grid branch creation
	UE_LOG(LogTemp, Log, TEXT("\n\n================= Finished ========================="));
}

