// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.

#pragma once

//#include "CoreMinimal.h"
#include "EFactionTypes.generated.h"

UENUM(BlueprintType)
enum class EFactionTeam : uint8 {
	ENone UMETA(DisplayName = "None"),                      //1
	EEnemyFaction UMETA(DisplayName = "Enemy Faction"),     //2
	EPlayer UMETA(DisplayName = "Player"),                  //3
	ENeutral  UMETA(DisplayName = "Neutral")                //4
};