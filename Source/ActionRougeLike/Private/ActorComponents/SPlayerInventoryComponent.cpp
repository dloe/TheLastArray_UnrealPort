// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "ActorComponents/SPlayerInventoryComponent.h"
#include "Player/SCharacter.h"
#include "Kismet/GameplayStatics.h"

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
			UpdateWeaponEquippedBool();
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
			UpdateWeaponEquippedBool();
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
				EquippedItem = nullptr;
				hasWeaponEquipped = false;
				EquipedWeaponFromInventory = nullptr;
			}
		}
	}

	return foundItem;
}

bool USPlayerInventoryComponent::EquipItemAtIndex(int indexToFind, AActor* Instigator)
{
	bool foundItem = false;
	PlayerA = Cast<ASCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	for (int index = 0; index < HotbarInventory.Num(); index++) //UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (index == indexToFind)
		{
			foundItem = true;
			EquippedItem = HotbarInventory[index]->ItemData;
			EquippedSlotIndex = index;
			//EquippedIdleAnim = HotbarInventory[index]->ItemData->IldeAnimWhenEquipped;

			EquipedWeaponFromInventory = Cast<USBaseWeapon>(EquippedItem);

			//if the hotbar weapon doesn't already have the physical weapon spawned, spawn it

			//run animation to swap weapons

			//for now will just have it spawn and immediately attach to player, no swap animation yet
			//despawn old weapon
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			//ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
			SpawnParams.Instigator = PlayerA;

			//@TODO: socket might change based on what item, take that into account (if weapon vs equippable item)
			FTransform socketTransform = PlayerA->GetMesh()->GetSocketTransform(EquippedItem->HandSocketName);
			EquippedItem->ItemActor = GetWorld()->SpawnActor<AActor>(EquippedItem->ItemActorSubclass, socketTransform, SpawnParams);
			

			//attach to socket
			EquippedItem->ItemActor->AttachToComponent(PlayerA->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			EquippedItem->HandSocketName);

			UpdateWeaponEquippedBool();

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
			EquipedWeaponFromInventory = Cast<USBaseWeapon>(EquippedItem);
			UpdateWeaponEquippedBool();
		}
	}

	return foundItem;
}

/// <summary>
/// Will first check if the equipped item is a weapon, if they have ammo to reload and then will assign the reload tag, run the 
/// weapons reload behavior and then unset the reload tag
/// </summary>
bool USPlayerInventoryComponent::CanReload()
{
	bool ReloadableStatus = false;

	UInventorySlot* ItemEquipped = GetEquippedItem();
	
	if (ItemEquipped->IsWeapon)
	{
		//USBaseWeapon* EquipedWeaponFromInventory = Cast<USBaseWeapon>(ItemEquipped->ItemData);
		if (EquipedWeaponFromInventory->CanBeReloaded())
		{
			ReloadableStatus = true;
		}
	}

	//perform action that then needs
	return ReloadableStatus;
}

bool USPlayerInventoryComponent::CanFireWeapon()
{
	bool CanFireWeapon = false;

	//USBaseWeapon* EquipedWeaponFromInventory = Cast<USBaseWeapon>(EquippedItem);
	//reload if mag is less than max capacity
	if (EquipedWeaponFromInventory->CurrentMagazineSize > 0 && 
		EquipedWeaponFromInventory->CurrentMagazineSize <= EquipedWeaponFromInventory->StandardMagazineSized)
	{
		hasWeaponEquipped = true;
		CanFireWeapon = true;
	}

	return CanFireWeapon;
}

/// <summary>
/// Get from hotbar
/// </summary>
/// <returns></returns>
UInventorySlot* USPlayerInventoryComponent::GetEquippedItem()
{
	if(EquippedSlotIndex < HotbarInventory.Num() && EquippedSlotIndex >= 0)
		return HotbarInventory[EquippedSlotIndex];
	else
		return nullptr;
}

/// <summary>
/// Override based on loadout data object
/// </summary>
void USPlayerInventoryComponent::LoadInventory(AActor* Instigator)
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
		EquipItemAtIndex(0, Instigator);
}

/// <summary>
/// When mag is able to be let go, we spawn a mag at the proper rotation and transform
/// More detail would be to make the mag we take from the gun disappear
/// </summary>
void USPlayerInventoryComponent::WeaponMagDropEvent()
{
	//@TODO: maybe add mags spawned to a first in first out queue that based on a setting, they slowely get deleted

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
	SpawnParams.Instigator = Cast<ASCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FTransform socketTransform = PlayerA->GetMesh()->GetSocketTransform(EquippedItem->HandSocketName);
	AActor* WeaponMag = GetWorld()->SpawnActor<AActor>(EquipedWeaponFromInventory->MagazineActor, socketTransform, SpawnParams);


}

/// <summary>
/// Ammo is now fully replenished, we can fire the gun again? or should it be the ammo is now updated in UI, still gotta finish the animation
/// 
/// @TODO: Maybe it could be cancelable at this stage?
/// </summary>
void USPlayerInventoryComponent::WeaponMagInEvent()
{

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

			//remove from corresponding index of places to spawn items

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

			//TODO: Depending on which index, corresponds to a different place for the aactor of the weapon to spawn

		}
	}

	return foundItem;
}

/// <summary>
/// Simply check if our equipped item is a weapon and update the hasWeaponEquipped state for animation
/// </summary>
/// <returns></returns>
void USPlayerInventoryComponent::UpdateWeaponEquippedBool()
{
	UInventorySlot* ItemEquipped = GetEquippedItem();

	hasWeaponEquipped = ItemEquipped!= nullptr ? ItemEquipped->IsWeapon : false;
}
