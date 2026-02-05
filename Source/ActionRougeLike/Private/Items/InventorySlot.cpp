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
/// This may need to be rethought or reorganized....
/// 
/// </summary>
/// <param name="itemToAssign"></param>
void UInventorySlot::AssignItem(UItemBase* itemToAssign)
{
	IsWeapon = false;
	IsEquipable = false;
	USBaseWeapon* PotentialWeapon = Cast< USBaseWeapon>(itemToAssign);
	if (PotentialWeapon)
	{
		IsWeapon = true;
		IsEquipable = true;
	}

	//@TODO check if consumable also here
	//

}
