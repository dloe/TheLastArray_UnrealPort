// Fill out your copyright notice in the Description page of Project Settings.


#include "Objectives/SBaseObjective.h"

// Sets default values
USBaseObjective::USBaseObjective()
{


}

void USBaseObjective::Initialize(ULevelAssetSetupComponent* LevelAssetSetupComp)
{
	LevelAssetSetupCompRef = LevelAssetSetupComp;

	//usually in children we will be doing stuff like spawning objective items, choosing sections for minibosses,
	//updating UI, assigning this to gamemode to its special parameter for checking, etc
}

bool USBaseObjective::CheckForObjectiveCompletion()
{
	bool ObjectiveStatus = false;

	return ObjectiveStatus;
}

void USBaseObjective::ObjectiveCompletion()
{
	
}

void USBaseObjective::SetupObjective()
{
	
}

