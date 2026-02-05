// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapons/SBaseWeapon.h"
#include "Items/UItemBase.h"
#include "Items/InventorySlot.h"
#include "Items/StockInventoryLoadoutsData.h"
#include "SBaseInventoryComponent.generated.h"

/// <summary>
/// Base inventory, will be overloaded for player and enemy AI
/// 
/// player inventory, handle storing of weapons, loot (goop), base materials and currency
/// enemy/other player inventory handles general drops on death
/// maybe apply to item chests?
/// 
/// Interface between retrieving or equipping items?
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API USBaseInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USBaseInventoryComponent();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//@TODO: This will eventually be an item (that weapons are a child class of)
	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	virtual UInventorySlot* GetEquippedItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool AddItemToInventory(UItemBase* ItemToAdd);

	//could pass in data loaded to assign to inventory?
	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	virtual void LoadInventory();

	//Some type of data structure to hold items
	//could include weapons? maybe have additional hotbar for weapons you can actually equip on character.
	//think similar to minecraft where we have a hotbar to choose from
	// 
	//could include weapons and usable items?
	//make array of item struct set to an original size of 6
	//player may need to make upgrades 

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int BaseInventorySize = 6;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<UInventorySlot*> Inventory;

	/// <summary>
	/// Count hotbar as inventory extension btw
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int TotalItemsInInventory = 0;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	void SwapInventorySlots(int IndexA, int IndexB);


	//don't need to move items really in base inventory, only player can do that

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	//whatever index is selected, access that one and show to UI for highlighting
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int EquippedSlotIndex = 0;

	//updated every time we add sometihng to inventory or remove
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int InventorySlotsAssigned = 0;
};
