// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Objectives/SBaseObjective.h"
#include "ULevelAssetSetupComponent.h"
#include "SMainGameMode.generated.h"

class USSaveGame;
class UDataTable;

/**
 * This is the gamemode for rounds after rounds in the rouge-lite
 * 
 * Player spawns on outskirts, game setup runs, objective is choosen and when the player extracts we go back to the train
 */
UCLASS()
class ACTIONROUGELIKE_API ASMainGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------
	UFUNCTION(BlueprintCallable, Category = "Objective")
	void CheckObjective();

	UFUNCTION(BlueprintCallable, Category = "Objective")
	void ExitLevel();

	//adds the loading screen in bp
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	void TransitionToTrainLevel();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void KillNormalEnemyEvent(AActor* InstigatorActor, int MinionCost, AActor* EnemyKilled);

	UFUNCTION(exec)
	void KillAll();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	void SetLevelSetupComp(ULevelAssetSetupComponent* comp) { LevelSetupComp = comp; };

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Setup")
	USBaseObjective* LevelObjective;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Setup")
	ASTileManager* TileManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Setup")
	ASLevelExit* LevelExitRef;

	

protected:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
	ULevelAssetSetupComponent* LevelSetupComp;


	//save info
	FString SlotName;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

};
