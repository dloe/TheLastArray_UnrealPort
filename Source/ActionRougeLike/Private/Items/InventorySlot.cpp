// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "Items/InventorySlot.h"
#include "Weapons/SBaseWeapon.h"

UInventorySlot::UInventorySlot()
{

}

void UInventorySlot::Initialize(UItemBase* itemDataP, bool isweaponP, bool isequipableP)
{
	if(itemDataP)
		ItemData = itemDataP;
	IsWeapon = isweaponP;
	IsEquipable = isequipableP;
}

/// <summary>
/// Setup for asigning items
/// 
/// </summary>
/// <param name="itemToAssign"></param>
void UInventorySlot::AssignItem(UItemBase* itemToAssign)
{
	ItemData = itemToAssign;
	IsEquipable = itemToAssign && itemToAssign->IsEquippable();
	IsWeapon = itemToAssign && itemToAssign->ItemType == EItemType::EWeapon;
	
	//@TODO check if consumable also here
}

USBaseWeapon* UInventorySlot::GetWeaponData()
{
	return (IsWeapon) ? Cast<USBaseWeapon>(ItemData) : nullptr;
}