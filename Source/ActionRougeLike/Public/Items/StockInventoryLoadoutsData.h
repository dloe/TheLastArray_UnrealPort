// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventorySlot.h"
#include "StockInventoryLoadoutsData.generated.h"

USTRUCT(BlueprintType)
struct FLoadout {
	GENERATED_BODY()

	public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<UInventorySlot*> HotbarInventory;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int BaseHotbarSize = 3;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int BaseInventorySize = 6;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<UInventorySlot*> Inventory;
};

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UStockInventoryLoadoutsData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FLoadout StockLoadout;
};
