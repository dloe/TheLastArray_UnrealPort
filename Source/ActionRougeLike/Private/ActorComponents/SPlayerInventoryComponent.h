// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/SBaseInventoryComponent.h"
#include "Animation/AnimSequence.h"
#include "Actions/SActionComponent.h"
//#include "Player/SCharacter.h"
#include "SPlayerInventoryComponent.generated.h"

class ASCharacter;

USTRUCT()
struct FBackInventory
{
	GENERATED_BODY()

	FBackInventory()
	{
		ItemActor = nullptr;
	}

	FBackInventory(FName name)
	{
		ItemStorageSocketName = name;
		ItemActor = nullptr;
	}

public:
	UPROPERTY()
	FName ItemStorageSocketName;

	UPROPERTY()
	AActor* ItemActor;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryLoad, EItemType, SwapType);

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
	void EquipItemAtIndex(int index);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool EquipItemByName(FName ItemName);

	//gives item data
	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	virtual UItemBase* GetEquippedItem();

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool CanReload();

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool CanFireWeapon();

	virtual UInventorySlot* GetEquippedSlot() override;

	virtual void OnDeathInventoryDrop() override;

	virtual void LoadInventory(AActor* Instigator) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory Setup")
	void SetPlayerActor(ASCharacter* playerToSet) { PlayerA  = playerToSet; };

	UFUNCTION(BlueprintCallable, Category = "Inventory Swap")
	void EquipItemBehavior();

	UFUNCTION(BlueprintCallable, Category = "Inventory Swap")
	void DeEquipItemBehavior();

	UFUNCTION(BlueprintCallable, Category = "Inventory Swap")
	void RemoveItemVisibilitiyByIndex(int IndexToRemove);

	//events specifically called from the anim blueprint
	//since i have alot of weapon data, it would be more organized to handle weapon event functions directly in the player invntory
	//might change to be directly in animbp
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Inventory Setup")
	void WeaponMagDropEvent();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Inventory Setup")
	void WeaponMagInEvent();

	UFUNCTION(BlueprintPure, Category = "Inventory State", meta = (DeprecatedFunction, DeprecationMessage = "Use NewFunction instead."))
	bool HasWeaponEquippedCheck();
	
	UFUNCTION(BlueprintCallable, Category = "Swap Item Action")
	void SetEquippedItem(UItemBase* NewItem);

	UFUNCTION(BlueprintCallable, Category = "Swap Item Action")
	int CurrentHotbarSlot() {return EquippedSlotIndex; };

	//TODO: refactoring - combine the two numbers? why are they separate?
	UFUNCTION(BlueprintCallable, Category = "Swap Item Action")
	void SetHotbarSlot(int newSlot) { EquippedSlotIndex = newSlot; };

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(BlueprintAssignable)
	FOnInventoryLoad OnInventoryLoad;

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
	TArray<FBackInventory> BackRifleStorage;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FBackInventory> BackSidearmStorage;

	//TODO: is this redundant? should be removed or refacted to opnly use the current weapon data
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool hasWeaponEquipped;

	UPROPERTY(EditAnywhere, Category = "Swap Item Behavior")
	int HotbarToSwapTo = 0;

	UPROPERTY(EditAnywhere, Category = "PlayerComp")
	USActionComponent* ActionComp;

	//utility functions to sweep usable items from inventory to hotbar

	//integration to swap through hotbar (maybe use 1-3 or scroll?)

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Left Hand Adjustments")
	bool bCanRunEquipBehavior;

protected:
	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool MoveItemFromHotbar(int IndexAInv, int IndexBHot);

	UFUNCTION(BlueprintCallable, Category = "Inventory Behavior")
	bool MoveItemIntoHotbar(int IndexAHot, int IndexBInv);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	UStaticMeshComponent* GetStaticMeshCompByName(AActor* ActorToCheck, FName CompName);

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Swap")
	EItemType PrevEquippedItemType = EItemType::ENone;

	UPROPERTY(BlueprintReadWrite, Category = "Item Swap")
	UItemBase* PrevItemRef;

	UPROPERTY(BlueprintReadWrite, Category = "Item Swap")
	int PrevItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bHasWeaponEquipped;
};
