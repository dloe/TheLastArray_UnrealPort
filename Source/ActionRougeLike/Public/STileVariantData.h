// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enviornment/TileGeneration/STileManager.h"
#include "STileVariantData.generated.h"

USTRUCT()
struct FTilePath
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<int> neighborArray;


};

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USTileVariantData : public UDataAsset
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray <int> ShapeDirections;

	//UPROPERTY(EditAnywhere)
	//ETileSizeVariant VariantType;
};
