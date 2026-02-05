// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "ActorComponents/SPlayerInventoryComponent.h"

USPlayerInventoryComponent::USPlayerInventoryComponent()
{

}



/// <summary>
/// Item is added to immediate hot-bar for use
/// </summary>
/// <param name="Weapon"></param>
/// <returns></returns>
bool USPlayerInventoryComponent::AddItemToEquipableHotbar(UItemBase* ItemToAdd)
{
	bool foundSpace = false;

	for (int index = 0; index < HotbarInventory.Num(); index++) //UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (HotbarInventory[index]->isEmptySlot())
		{
			foundSpace = true;
			HotbarInventory[index]->AssignItem(ItemToAdd);
		}
	}

	return foundSpace;
}

/// <summary>
/// Still may need to wait to look at this one. If we are removing it form inventory would it be deleting?
/// 
/// I mean if im removing this to drop from inventory and have it exist in level as a dropped item, then i wont pass by ref
/// </summary>
/// <param name="ItemToRemove"></param>
/// <returns></returns>
bool USPlayerInventoryComponent::RemoveItemFromInventory(UItemBase* ItemToRemove)
{
	bool foundItem = false;

	for (int index = 0; index < Inventory.Num(); index++) //UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (Inventory[index]->ItemData == ItemToRemove)
		{
			foundItem = true;
			Inventory.RemoveAt(index);
		}
	}

	return foundItem;
}

bool USPlayerInventoryComponent::RemoveItemToEquipableHotbar(UItemBase* EquippedItemToRemove)
{
	bool foundItem = false;

	for (int index = 0; index < HotbarInventory.Num(); index++) //UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (HotbarInventory[index]->ItemData == EquippedItemToRemove)
		{
			foundItem = true;
			HotbarInventory.RemoveAt(index);
			if (EquippedSlotIndex == index)
			{
				//find next open slot to reassign as equipped slot
				//if (HotbarSlotsAssigned > 0)
				//{
					//for (int indexSub = 0; indexSub < HotbarInventory.Num(); indexSub++) //UInventorySlot* Slot : Inventory)
					//{
						//if (!HotbarInventory[index]->isEmptySlot())
						//{
						//	EquippedItem = HotbarInventory[index]->ItemData;
						//	EquippedSlotIndex = index;
						//}
					//}
				//}
				//else { //nothing in hotbar so donut show anything
					//HotbarSlotsAssigned = 0;
					EquippedItem = nullptr;
				//}
			}
		}
	}

	return foundItem;
}

bool USPlayerInventoryComponent::EquipItemAtIndex(int indexToFind)
{
	bool foundItem = false;

	for (int index = 0; index < HotbarInventory.Num(); index++) //UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (index == indexToFind)
		{
			foundItem = true;
			EquippedItem = HotbarInventory[index]->ItemData;
			EquippedSlotIndex = index;
		}
	}

	return foundItem;
}

bool USPlayerInventoryComponent::EquipItemByName(FName ItemName)
{
	bool foundItem = false;

	for (int index = 0; index < HotbarInventory.Num(); index++) //UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (HotbarInventory[index]->ItemData->ItemName == ItemName)
		{
			foundItem = true;
			EquippedItem = HotbarInventory[index]->ItemData;
			EquippedSlotIndex = index;
		}
	}

	return foundItem;
}

/// <summary>
/// Get from hotbar
/// </summary>
/// <returns></returns>
UInventorySlot* USPlayerInventoryComponent::GetEquippedItem()
{
	ensure(EquippedSlotIndex < HotbarInventory.Num() && EquippedSlotIndex >= 0);

	return HotbarInventory[EquippedSlotIndex];
}

/// <summary>
/// Override based on loadout data object
/// </summary>
void USPlayerInventoryComponent::LoadInventory()
{
	BaseHotbarSize = LoadoutBaseData->StockLoadout.BaseHotbarSize;
	HotbarInventory.Empty();
	for(FSlot hotbarSlot : LoadoutBaseData->StockLoadout.HotbarInventory)
	{
		UInventorySlot* SlotToAdd = NewObject<UInventorySlot>(GetOwner()); //this is passed in so playerinventorycomp is the outer and owner?
		UItemBase* itemToAdd = nullptr;
		if(hotbarSlot.ItemDataClass) {
			itemToAdd = NewObject<UItemBase>(GetOwner(), hotbarSlot.ItemDataClass);
			TotalItemsInHotbar++;
			TotalItemsInInventory++;
		}
		SlotToAdd->Initialize(itemToAdd, hotbarSlot.IsWeapon, hotbarSlot.IsEquipable);
		HotbarInventory.Add(SlotToAdd);
	}
	BaseInventorySize = LoadoutBaseData->StockLoadout.BaseInventorySize;
	Inventory.Empty();
	for (FSlot invSlot : LoadoutBaseData->StockLoadout.Inventory)
	{
		UInventorySlot* SlotToAdd = NewObject<UInventorySlot>(this);
		UItemBase* itemToAdd = nullptr;
		if(invSlot.ItemDataClass) {
			itemToAdd = NewObject<UItemBase>(GetOwner(), invSlot.ItemDataClass);
			TotalItemsInInventory++;
		}
		SlotToAdd->Initialize(itemToAdd, invSlot.IsWeapon, invSlot.IsEquipable);
		Inventory.Add(SlotToAdd);
	}
	if(TotalItemsInHotbar > 0)
		EquipItemAtIndex(0);
}

/// <summary>
/// Moves item (or weapon) into base inventory FROM hotbar
/// </summary>
/// <param name="IndexAInv"></param>
/// <param name="IndexBHot"></param>
bool USPlayerInventoryComponent::MoveItemFromHotbar(int IndexAInv, int IndexBHot)
{
	bool foundItem = false;

	for (int index = 0; index < HotbarInventory.Num(); index++) //UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (HotbarInventory[index]->isEmptySlot())
		{
			foundItem = true;
			ensure(IndexBHot < HotbarInventory.Num() && IndexBHot >= 0);
			ensure(IndexAInv < Inventory.Num() && IndexAInv >= 0);
			UInventorySlot* tempCopy = HotbarInventory[IndexBHot];
			//ensure(!tempCopy->isEmptySlot()); //might not need this check

			HotbarInventory[IndexBHot] = Inventory[IndexAInv];
			Inventory[IndexAInv] = tempCopy;
		}
	}

	return foundItem;
}

/// <summary>
/// Moves item or weapon into hotbar FROM inventory
/// move into first available hotbar?
/// 
/// or maybe have player identify spot to move back? for now just take first available
/// </summary>
/// <param name="IndexAHot"></param>
/// <param name="IndexBInv"></param>
bool USPlayerInventoryComponent::MoveItemIntoHotbar(int IndexAHot, int IndexBInv)
{
	//move into first available for now
	bool foundItem = false;

	for (int index = 0; index < HotbarInventory.Num(); index++) //UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (HotbarInventory[index]->isEmptySlot())
		{
			foundItem = true;
			ensure(IndexAHot < HotbarInventory.Num() && IndexAHot >= 0);
			ensure(IndexBInv < Inventory.Num() && IndexBInv >= 0);
			UInventorySlot* tempCopy = HotbarInventory[IndexAHot];
			//ensure(!tempCopy->isEmptySlot()); //might not need this check

			HotbarInventory[IndexAHot] = Inventory[IndexBInv];
			Inventory[IndexBInv] = tempCopy;
		}
	}

	return foundItem;
}
