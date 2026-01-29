// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "Objectives/SEliminationObjective.h"

USEliminationObjective::USEliminationObjective()
{
	//OnEnemySpawnCompletedEvent.AddDynamic(this, &USEliminationObjective::OnEnemySpawned);
	ObjectiveName = "Elimination";
	ObjectiveDescription = "Eliminate the enemies.";

}

void USEliminationObjective::Initialize(ULevelAssetSetupComponent* LevelAssetSetupComp)
{
	Super::Initialize(LevelAssetSetupComp);
	
}

bool USEliminationObjective::CheckForObjectiveCompletion()
{
	bool ObjectiveStatus = false;
	//14 total, need to get 11 kills so num should be <= 14 - 11
	if (LevelAssetSetupCompRef->SpawnedEnemiesInLevel.Num() <= EnemyStartingCount - KillCountRequirement)
	{
		ObjectiveStatus = true;
	}
	return ObjectiveStatus;
}

void USEliminationObjective::OnEnemySpawned()
{
	UE_LOG(LogTemp, Log, TEXT("Eleminaition Objective Setup"));

	//for now kill only ~75% of the enemies
	KillCountRequirement = LevelAssetSetupCompRef->EnemiesPlaced * 0.75f;
	EnemyStartingCount = LevelAssetSetupCompRef->EnemiesPlaced;

	//finish setup for this objective (had to wait until the enemies where spawned

	UE_LOG(LogTemp, Log, TEXT("Eliminate %d Enemies"), KillCountRequirement);
}
