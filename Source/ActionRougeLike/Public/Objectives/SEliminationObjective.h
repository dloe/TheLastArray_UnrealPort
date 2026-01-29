// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Objectives/SBaseObjective.h"
#include "Enviornment/ULevelAssetSetupComponent.h"
#include "SEliminationObjective.generated.h"



/**
 * Goal is to eliminate a certain percent of the enemies. At the around halfway maybe more could spawn but
 * for now its just a percent off the main spawned number.
 * 
 * Will collect the total enemies spawned number on creation
 * Maybe mini bosses can spawn also guaranteed at higher levels
 */
UCLASS()
class ACTIONROUGELIKE_API USEliminationObjective : public USBaseObjective
{
	GENERATED_BODY()
	
public:

	USEliminationObjective();

	// ---------------------------------
	// -------- Public Functions -------
	// ---------------------------------

	//UFUNCTION(BlueprintCallable, Category = "Objective")
	virtual void Initialize(ULevelAssetSetupComponent* LevelAssetSetupComp) override;

	virtual bool CheckForObjectiveCompletion();

	//todo: maybe spawn this function with SetUpObjective? thats its purpose right?
	UFUNCTION()
	void OnEnemySpawned();

	// ---------------------------------
	// ------- Public Variables --------
	// ---------------------------------

	//will determine how many kills remain based on something else, should the player keep track per level?
	//how many kills total
	//TODO: consider how this will change when we spawn in more enemies mid game
	UPROPERTY(EditAnywhere, Category = "Objective")
	int KillCountRequirement;

	UPROPERTY(EditAnywhere, Category = "Objective")
	int EnemyStartingCount = 0;

	//may need to change this later but will see how it works as a delegate event design
	//event to get the enemy spawn info for objective to finish setting up

	
protected:

	// ---------------------------------
	// -- Internal state / variables  --
	// ---------------------------------
	


	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	

};
