// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/SBaseInventoryComponent.h"
#include "SPlayerInventoryComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USPlayerInventoryComponent : public USBaseInventoryComponent
{
	GENERATED_BODY()

	USPlayerInventoryComponent();

public:

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------
	//@TODO: should this be a more general class that weapon inherits from?
	//UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	//bool AddItemToInventory(UItemBase* ItemToAdd);

	//@TODO: should this be a more general class that weapon inherits from?
	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool AddItemToEquipableHotbar(UItemBase* ItemToAdd);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool RemoveItemFromInventory(UItemBase* ItemToRemove);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool RemoveItemToEquipableHotbar(UItemBase* ItemToEquip);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool EquipItemAtIndex(int index);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool EquipItemByName(FName ItemName);

	virtual UInventorySlot* GetEquippedItem() override;

	virtual void LoadInventory() override;

	//@TODO: This will eventually be an item (that weapons are a child class of)
	//UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	//ASBaseWeapon* GetEquippedWeapon();
	

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	//data structure to incorporate hot bar
	//@TODO: should hotbar be an extension of the inventory? or be x amount of highlighted references to 
	//slots already existing in the inventory?
	//players like space, make it an extension
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int BaseHotbarSize = 3;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<UInventorySlot*> HotbarInventory;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	UStockInventoryLoadoutsData* LoadoutBaseData;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int TotalItemsInHotbar = 0;

	//utility functions to sweep usable items from inventory to hotbar

	//integration to swap through hotbar (maybe use 1-3 or scroll?)

protected:
	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool MoveItemFromHotbar(int IndexAInv, int IndexBHot);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool MoveItemIntoHotbar(int IndexAHot, int IndexBInv);

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
	//@todo: this will eventually be an item that weapon is a child of
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	UItemBase* EquippedItem;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int HotbarSlotsAssigned = 0;
};
