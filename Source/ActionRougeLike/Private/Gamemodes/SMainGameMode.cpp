// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemodes/SMainGameMode.h"

void ASMainGameMode::CheckObjective()
{
	if (LevelObjective->CheckForObjectiveCompletion())
	{
		//returns true, objective complete can now extract

	}
}
