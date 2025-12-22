// Fill out your copyright notice in the Description page of Project Settings.

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
