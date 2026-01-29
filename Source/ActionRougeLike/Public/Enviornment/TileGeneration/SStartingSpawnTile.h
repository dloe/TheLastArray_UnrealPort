// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Enviornment/TileGeneration/STile.h"
#include "Interactables/SLevelExit.h"
#include "SStartingSpawnTile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASStartingSpawnTile : public ASTileVariantEnviornment
{
	GENERATED_BODY()


	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Components - Neighbors")
	FTransform DoorSpawnPoint;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Setup")
	ASLevelExit* LevelExitRef;

};
