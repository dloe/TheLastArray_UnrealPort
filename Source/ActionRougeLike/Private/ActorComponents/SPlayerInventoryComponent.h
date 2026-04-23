// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/SBaseInventoryComponent.h"
//#include "Player/SCharacter.h"
#include "SPlayerInventoryComponent.generated.h"

class ASCharacter;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnWeaponEquipped, EItemType, PrevEquippedItemType, EItemType, NewEquippedItemType, UAnimSequence*, PrevItemUnequipSeq, UAnimSequence*, NewItemEquipMontagSeq);


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
	bool EquipItemAtIndex(int index, AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool EquipItemByName(FName ItemName);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool CanReload();

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool CanFireWeapon();

	virtual UInventorySlot* GetEquippedItem() override;

	virtual void LoadInventory(AActor* Instigator) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory Setup")
	void SetPlayerActor(ASCharacter* playerToSet) { PlayerA  = playerToSet; };


	//events specifically called from the anim blueprint
	//since i have alot of weapon data, it would be more organized to handle weapon event functions directly in the player invntory
	//might change to be directly in animbp
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Inventory Setup")
	void WeaponMagDropEvent();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Inventory Setup")
	void WeaponMagInEvent();

	//@TODO: This will eventually be an item (that weapons are a child class of)
	//UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	//ASBaseWeapon* GetEquippedWeapon();

	UFUNCTION(BlueprintPure, Category = "Inventory State")
	bool HasWeaponEquipped() const;
	

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(BlueprintAssignable)
	FOnWeaponEquipped OnWeaponEquipped;

	//data structure to incorporate hot bar
	//@TODO: should hotbar be an extension of the inventory? or be x amount of highlighted references to 
	//slots already existing in the inventory?
	//players like space, make it an extension
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int BaseHotbarSize = 3;

	//TODO: Make inventory a map based on index, faster lookups
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<UInventorySlot*> HotbarInventory;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	UStockInventoryLoadoutsData* LoadoutBaseData;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int TotalItemsInHotbar = 0;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int CurrentHotbarIndex = 0;

	//TODO: is this redundant? should be removed or refacted to opnly use the current weapon data
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool hasWeaponEquipped;

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

	//UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	//void UpdateWeaponEquippedBool();

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Attack")
	USBaseWeapon* EquipedWeaponFromInventory;
	
	//@todo: this will eventually be an item that weapon is a child of
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	UItemBase* EquippedItem;

	//TBD: potential upgrade? maybe this is saved and loaded in else where. Hotbar size should be dynamic and recorded as so
	//UPROPERTY(EditAnywhere, Category = "Inventory")
	//int HotbarSlotsAssigned = 0;

	UPROPERTY(EditAnywhere, Category = "Player")
	ASCharacter* PlayerA;

	//Helps with transitions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	EItemType PrevEquippedItemType = EItemType::ENone;
};
