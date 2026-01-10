// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Objectives/SBaseObjective.h"
#include "SMainGameMode.generated.h"

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
	UPROPERTY(EditAnywhere, Category = "Objective")
	void CheckObjective();

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	USBaseObjective* LevelObjective;


protected:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------


	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------


};
