// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Items/InventorySlot.h"
#include "StockInventoryLoadoutsData.generated.h"

USTRUCT(BlueprintType)
struct FSlot {
	GENERATED_BODY()

	public:

	//must be subclass of because the itemdata isnt spawned in teh real world right? its a uobject
	//its not really created and exposed to the editor to see and reference
	//the blueprint is what we want tehrefore it has to be the one we use
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UItemBase> ItemDataClass; //winner winner chicken AHHHHHHH

	//TBD what else
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWeapon;

	//maybe this one over IsWeapon bool?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsEquipable;
};

USTRUCT(BlueprintType)
struct FLoadout {
	GENERATED_BODY()

	public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FSlot> HotbarInventory;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int BaseHotbarSize = 3;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int BaseInventorySize = 6;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FSlot> Inventory;

};

/**
 * 
 */
UCLASS(BlueprintType)
class ACTIONROUGELIKE_API UStockInventoryLoadoutsData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FLoadout StockLoadout;
};
