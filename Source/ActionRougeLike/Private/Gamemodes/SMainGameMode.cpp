// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemodes/SMainGameMode.h"

void ASMainGameMode::CheckObjective()
{
	if (LevelObjective->CheckForObjectiveCompletion())
	{
		//returns true, objective complete can now extract
		UE_LOG(LogTemp, Log, TEXT("OBJECTIVE COMPLETED. PLAYER WILL BE ABLE TO NOW EXTRACT"));
	}
}
