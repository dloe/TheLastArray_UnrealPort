// Fill out your copyright notice in the Description page of Project Settings.


#include "UTileGridBranchComponent.h"
#include "Components/ActorComponent.h"
#include "Enviornment/TileGeneration/STileManager.h"

// Sets default values for this component's properties
UTileGridBranchComponent::UTileGridBranchComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}

/// <summary>
/// This is called from STileManager after initial grid and path is setup.
/// 
/// Calls modules to handle random rooms, random branches, single rooms, secret rooms, 
/// final doors setup and deactivating inactive rooms
/// </summary>
void UTileGridBranchComponent::GameMapAdditionalSetup()
{
	TileManagerRef = Cast<ASTileManager>(GetOwner());
	LocalLevelRef = TileManagerRef->MyLocalLevel;

	UE_LOG(LogTemp, Log, TEXT("-----------------------------------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("========== Grid Additions and Final Setup ================="));
	UE_LOG(LogTemp, Log, TEXT("-----------------------------------------------------------"));

	RandomRoomsAndBranchesAdditions();

	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished Random Rooms - Adding Spawn Room =============================="));



	AddSingleRooms();

	if (LocalLevelRef->CurrentLevelTier >= ELevelTier::ELevel_2) {
		if (TileManagerRef->DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("=================== Finished Spawn Room - Adding Secret Room =============================="));
		CreateSecretRoom();

	}


	if (TileManagerRef->DoorsActive) {
		if (TileManagerRef->DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("=================== Finished Secret Room - Activating All Doors =============================="));

		FinalDoorSetupDoors();
	}

	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished All Doors - Implementing Final Tile Setup =============================="));

	DeactiveInactiveRooms();




	//notify next component to run
	OnGridAdditionalSetupCompletedEvent.Broadcast();
}

// Called when the game starts
void UTileGridBranchComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



void UTileGridBranchComponent::RandomRoomsAndBranchesAdditions()
{

}

void UTileGridBranchComponent::AddSingleRooms()
{

}

void UTileGridBranchComponent::CreateSecretRoom()
{

}

void UTileGridBranchComponent::FinalDoorSetupDoors()
{

}

void UTileGridBranchComponent::DeactiveInactiveRooms()
{

}

