// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SLocalLevel.h"
//#include "ULevelAssetSetupComponent.h"
#include "SBaseObjective.generated.h"

class ULevelAssetSetupComponent;

/// <summary>
/// Base Objective class
/// 
/// Keeps track of if this specific objective is completed
/// Holds data for objective (description, references to pickups needed, etc)
/// Will be accessible via the gamemode but choosen via the ULevelAssetSetupComp
/// Whenever something is triggered, or happens we will call a function to verify if the objective has been met
/// Objective completed function that is overridden by each child
/// Each objective has a setup procedure as well
/// 
/// EVERYONE SHOULD OVERRIDE THIS basically
/// </summary>
UCLASS()
class ACTIONROUGELIKE_API USBaseObjective : public UObject
{
	GENERATED_BODY()
	
public:	

	// ---------------------------------
	// -------- Public Functions -------
	// ---------------------------------

	// Sets default values for this actor's properties
	USBaseObjective();

	//UFUNCTION(BlueprintCallable, Category = "Objective")
	virtual void Initialize(ULevelAssetSetupComponent* LevelAssetSetupComp);

	UFUNCTION(BlueprintCallable, Category = "Objective")
	virtual bool CheckForObjectiveCompletion();

	UFUNCTION(BlueprintCallable, Category = "Objective")
	virtual void ObjectiveCompletion();

	UFUNCTION(BlueprintCallable, Category = "Objective")
	virtual void SetupObjective();

	// ---------------------------------
	// ------- Public Variables --------
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Objective Data")
	bool ObjectiveComplete = false;

	UPROPERTY(EditAnywhere, Category = "Objective Data")
	FString ObjectiveName;

	UPROPERTY(EditAnywhere, Category = "Objective Data")
	FString ObjectiveDescription;

	UPROPERTY(EditAnywhere, Category = "Objecive Info")
	ELevelTier LevelTier;

	//reference to current gamemode class?

	//maybe an icon or symbol later for UI?

	//TODO: need some measurement of difficulty that could scale up
	//like a separate modifier?
	UPROPERTY(EditAnywhere, Category = "Objective Data")
	float DifficultyWeight;

	//TODO: Subobjectives!
	//could be list of subobjectives

protected:
UPROPERTY(EditAnywhere, Category = "Objective Data")
	ULevelAssetSetupComponent* LevelAssetSetupCompRef;
};
