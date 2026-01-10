// Fill out your copyright notice in the Description page of Project Settings.


#include "Objectives/SEliminationObjective.h"

USEliminationObjective::USEliminationObjective()
{
	//OnEnemySpawnCompletedEvent.AddDynamic(this, &USEliminationObjective::OnEnemySpawned);
}

void USEliminationObjective::Initialize(ULevelAssetSetupComponent* LevelAssetSetupComp)
{
	Super::Initialize(LevelAssetSetupComp);
	
}

bool USEliminationObjective::CheckForObjectiveCompletion()
{
	bool ObjectiveStatus = false;
	if (LevelAssetSetupCompRef->SpawnedEnemiesInLevel.Num() <= KillCountRequirement)
	{
		ObjectiveStatus = true;
	}
	return ObjectiveStatus;
}

void USEliminationObjective::OnEnemySpawned()
{
	UE_LOG(LogTemp, Log, TEXT("Eleminaition Objective Setup"));

	//for now kill only 75% of the enemies
	KillCountRequirement = LevelAssetSetupCompRef->EnemiesPlaced * 0.75f;

	//finish setup for this objective (had to wait until the enemies where spawned

	UE_LOG(LogTemp, Log, TEXT("Eleminiate %d Enemies"), KillCountRequirement);
}
