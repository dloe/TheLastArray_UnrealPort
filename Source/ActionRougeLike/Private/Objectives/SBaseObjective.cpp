// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

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

