// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SItemInteractableData.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USItemInteractableData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Info")
	TSubclassOf<AActor> ItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon;

	/// <summary>
	/// every asset in asset manager must have a primary asset id. ID is lightweight struct that IDs the asset at runtime
	/// 
	/// "Items" is primary asset type, must match type registered in DefaultGame.ini under PrimaryAssetTRypes
	/// GetFName is assets object name
	/// </summary>
	/// <returns></returns>
	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Items", GetFName());
	}
};
