// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupAssetData.generated.h"

USTRUCT()
struct FItemPickupAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, Category = "Item Info")
	int ItemWeight;

	UPROPERTY(EditAnywhere, Category = "Item Info")
	FString ItemName;

	//asset to spawn

}

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UPickupAssetData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	//each value is added to a local array in ULevelAsset that we then use those weights to randomly select
	UPROPERTY(EditAnywhere, Category = "Level Setup")
	TArray<FItemPickupAsset> ItemPickupTable; 

	//UPROPERTY(EditAnywhere, Category = "Level Setup")
	//int TotalItemWeight;
};
