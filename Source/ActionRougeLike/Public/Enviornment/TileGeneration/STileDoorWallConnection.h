// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "ASTileWall.h"
#include "STileDoorWallConnection.generated.h"

class ASTileDoor;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASTileDoorWallConnection : public ASTileWall
{
	GENERATED_BODY()
	

public:

	ASTileDoorWallConnection();

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	ASTileDoor* AMyTileDoor;

	//could be protected?
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	AActor* DoorKey;

	
	//for ref mainly
	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tile Generation")
	//float DoorwayLengthAdjustment = 200;

protected:


};
