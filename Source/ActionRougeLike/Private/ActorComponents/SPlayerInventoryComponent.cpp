// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "ActorComponents/SPlayerInventoryComponent.h"
#include "Player/SCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

USPlayerInventoryComponent::USPlayerInventoryComponent()
{
	PlayerA = Cast<ASCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//maybe could eventually separate by item type? rifles go on back and pistols go on hip?
	BackRifleStorage = {
	FBackInventory("RifleWeaponStash1"),
	FBackInventory("RifleWeaponStash2")
	};

	BackSidearmStorage = {
	FBackInventory("SidearmWeaponStash1"),
	FBackInventory("SidearmWeaponStash2")
	};
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
			//UpdateWeaponEquippedBool();
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
			//UpdateWeaponEquippedBool();
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
				//hasWeaponEquipped = false;
				EquipedWeaponFromInventory = nullptr;
			}
		}
	}

	return foundItem;
}

/// <summary>
/// TODO: This needs to be reworked to account for the weapon swapping event
/// </summary>
/// <param name="indexToFind">Item index we are swapping to. The new item</param>
/// <param name="Instigator"></param>
/// <returns></returns>
void USPlayerInventoryComponent::EquipItemAtIndex(int indexToFind)
{

	//TODO some redundancy check hence the bool, but needs more testing
	HotbarToSwapTo = indexToFind;

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
			//UpdateWeaponEquippedBool();
		}
	}

	return foundItem;
}

UItemBase* USPlayerInventoryComponent::GetEquippedItem()
{
	return EquippedItem;
}

/// <summary>
/// Will first check if the equipped item is a weapon, if they have ammo to reload and then will assign the reload tag, run the 
/// weapons reload behavior and then unset the reload tag
/// </summary>
bool USPlayerInventoryComponent::CanReload()
{
	bool ReloadableStatus = false;
	UInventorySlot* ItemEquipped = GetEquippedSlot();
	
	if (ItemEquipped->IsWeapon)
	{
		if (EquipedWeaponFromInventory->CanBeReloaded())
		{
			ReloadableStatus = true;
			UE_LOG(LogTemp, Log, TEXT("Weapon can be reloaded!"));
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
	if(EquipedWeaponFromInventory != nullptr) {
		if (EquipedWeaponFromInventory->CurrentMagazineSize > 0 && 
		EquipedWeaponFromInventory->CurrentMagazineSize <= EquipedWeaponFromInventory->StandardMagazineSized)
		{
			//hasWeaponEquipped = true;
			CanFireWeapon = true;
		}
	}

	return CanFireWeapon;
}

/// <summary>
/// Get from hotbar
/// </summary>
/// <returns></returns>
UInventorySlot* USPlayerInventoryComponent::GetEquippedSlot()
{
	if(EquippedSlotIndex < HotbarInventory.Num() && EquippedSlotIndex >= 0)
		return HotbarInventory[EquippedSlotIndex];
	else
		return nullptr;
}

//detach players weapon to fall on ground
void USPlayerInventoryComponent::OnDeathInventoryDrop()
{

	if(EquippedItem) {
		EquippedItem->ItemActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		UStaticMeshComponent* itemMesh = EquippedItem->GetItemStaticMesh();
		EquippedItem->ItemActor->SetActorEnableCollision(true);

		itemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		itemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		itemMesh->SetSimulatePhysics(false);
		itemMesh->SetSimulatePhysics(true);

		itemMesh->SetEnableGravity(true);

	}
}

/// <summary>
/// Override based on loadout data object
/// </summary>
void USPlayerInventoryComponent::LoadInventory(AActor* Instigator)
{
	BaseHotbarSize = LoadoutBaseData->StockLoadout.BaseHotbarSize;
	//HotbarInventory.Empty();
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
	if(TotalItemsInHotbar > 0) {
		EquipItemAtIndex(0);

		SetEquippedItem(HotbarInventory[0]->ItemData);
		OnInventoryLoad.Broadcast(HotbarInventory[0]->ItemData->ItemType);
		EquipItemBehavior();
	}
}

/// <summary>
/// Spawns in new item that we are swapping to
/// </summary>
void USPlayerInventoryComponent::EquipItemBehavior()
{
	PlayerA = Cast<ASCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//@TODO: socket might change based on what item, take that into account (if weapon vs equippable item)
	FTransform socketTransform = PlayerA->GetMesh()->GetSocketTransform(EquippedItem->HandSocketName); //hit here

	if(EquippedItem->ItemActor == nullptr) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = PlayerA;

		EquippedItem->ItemActor = GetWorld()->SpawnActor<AActor>(
			EquippedItem->ItemActorSubclass, FTransform::Identity, SpawnParams);
	}
	else {

		// didnt have a slot for it if -1 so just turn it back on again (-1 means it was not visible)
		if(EquippedItem->backSlot == -1) {
			EquippedItem->ItemActor->GetRootComponent()->SetVisibility(true, true);
			EquippedItem->ItemActor->SetActorTransform(socketTransform); //set back to hand transform
		}
		// reatach to socketTransform

		switch (HotbarInventory[PrevItemIndex]->ItemData->ItemType)
		{
			case EItemType::EWeaponRifle:
			case EItemType::EWeaponShotgun:
				BackRifleStorage[EquippedItem->backSlot].ItemActor = nullptr;
				break;
			case EItemType::EWeaponHandheld:
				BackSidearmStorage[EquippedItem->backSlot].ItemActor = nullptr;
				break;
		}
		EquippedItem->backSlot = -1;
	}
	
	//attach to socket
	EquippedItem->ItemActor->AttachToComponent(
		PlayerA->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, EquippedItem->HandSocketName);
	EquippedItem->ItemActor->SetActorRelativeScale3D(FVector(1.0f, 1.0f, 1.0f)); //revert scaling to default 1,1,1 some reason reattaching messes that up

	//assign if its a weapon
	EquipedWeaponFromInventory = Cast<USBaseWeapon>(EquippedItem);

	//Action turns off after swap delay set in swap item action (combination of whatever seq lengths is used else instant turn off)
}

/// <summary>
/// Despawns old item we are swapping away from
/// TODO: Might be a better way to swap out weapon actors
/// </summary>
void USPlayerInventoryComponent::DeEquipItemBehavior()
{
	RemoveItemVisibilitiyByIndex(PrevItemIndex);
}

/// <summary>
/// When we remove equipped weapon we move it to back or turn off visibility
/// </summary>
/// <param name="IndexToRemove"></param>
void USPlayerInventoryComponent::RemoveItemVisibilitiyByIndex(int IndexToRemove)
{
	//todo: some type of redundancy check to ensure its already take care of


	//find first slot available
	//if no slot available then doesn't matter
	bool hasSlot = false;
	//TODO: this might be refactored to just have one for loop with the vars swapped out for each type? what about for melee? maybe keep it as is for nwo
	switch (HotbarInventory[PrevItemIndex]->ItemData->ItemType)
	{
		case EItemType::EWeaponRifle:
		case EItemType::EWeaponShotgun:
			for (int s = 0; s < BackRifleStorage.Num(); s++)
			{
				if (BackRifleStorage[s].ItemActor == nullptr && BackRifleStorage[s].ItemStorageSocketName != "")
				{
					HotbarInventory[PrevItemIndex]->ItemData->ItemActor->AttachToComponent(PlayerA->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
						BackRifleStorage[s].ItemStorageSocketName);
					BackRifleStorage[s].ItemActor = HotbarInventory[PrevItemIndex]->ItemData->ItemActor;
					HotbarInventory[PrevItemIndex]->ItemData->backSlot = s;
					hasSlot = true;

					//do we have to set the transform for the actor
					UStaticMeshComponent* WeaponBaseSM = GetStaticMeshCompByName(HotbarInventory[PrevItemIndex]->ItemData->ItemActor, "SM_BaseWeapon");
					if(WeaponBaseSM) {
						//relative location (so distance away from weapon pivot)
						FTransform socketCenter_Rifle = WeaponBaseSM->GetSocketTransform("StorageCenter", RTS_Component);

						//will have to be adjusted on a per weapon basis to align the socketCenter_Rifle
						FVector OffsetLocal = -socketCenter_Rifle.GetLocation();
						FRotator OffsetRot = (-socketCenter_Rifle.GetRotation()).Rotator();

						HotbarInventory[PrevItemIndex]->ItemData->ItemActor->SetActorRelativeLocation(OffsetLocal);
						HotbarInventory[PrevItemIndex]->ItemData->ItemActor->SetActorRelativeRotation(OffsetRot);

					}
					else {
						//just turn off visibility then
						UE_LOG(LogTemp, Error, TEXT("Failed to get Sub SM on rifle type. Cant center weapon on back... [No SM found on weapon: %s]"),
							*GetNameSafe(HotbarInventory[PrevItemIndex]->ItemData->ItemActor));
						hasSlot = false;
					}
					break;
				}
			}
			break;
		case EItemType::EWeaponHandheld:
			for (int s = 0; s < BackSidearmStorage.Num(); s++)
			{
				if (BackSidearmStorage[s].ItemActor == nullptr && BackSidearmStorage[s].ItemStorageSocketName != "")
				{
					HotbarInventory[PrevItemIndex]->ItemData->ItemActor->AttachToComponent(PlayerA->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
						BackSidearmStorage[s].ItemStorageSocketName);
					BackSidearmStorage[s].ItemActor = HotbarInventory[PrevItemIndex]->ItemData->ItemActor;
					HotbarInventory[PrevItemIndex]->ItemData->backSlot = s;
					hasSlot = true;

					UStaticMeshComponent* WeaponBaseSM = GetStaticMeshCompByName(HotbarInventory[PrevItemIndex]->ItemData->ItemActor, "SM_BaseWeapon");
					if (WeaponBaseSM) {
						FTransform socketCenter_Sidearm = WeaponBaseSM->GetSocketTransform("StorageCenter", RTS_Component);

						//will have to be adjusted on a per weapon basis to align the socketCenter_Rifle
						FVector OffsetLocal = -socketCenter_Sidearm.GetLocation();
						FRotator OffsetRot = (-socketCenter_Sidearm.GetRotation()).Rotator();

						HotbarInventory[PrevItemIndex]->ItemData->ItemActor->SetActorRelativeLocation(OffsetLocal);
						HotbarInventory[PrevItemIndex]->ItemData->ItemActor->SetActorRelativeRotation(OffsetRot);
					}
					else {
						//just turn off visibility then
						UE_LOG(LogTemp, Error, TEXT("Failed to get Sub SM on sidearm type. Cant center weapon on back... [No SM found on sidearm: %s]"),
							*GetNameSafe(HotbarInventory[PrevItemIndex]->ItemData->ItemActor));
						hasSlot = false;
					}
					break;
				}
			}
			break;
	}

	//if no slot available the nwe turn off the model visibility and put it at at adefault slot?
	if (!hasSlot)
	{
		HotbarInventory[PrevItemIndex]->ItemData->backSlot = -1;
		HotbarInventory[PrevItemIndex]->ItemData->ItemActor->GetRootComponent()->SetVisibility(false, true);
		HotbarInventory[PrevItemIndex]->ItemData->ItemActor->AttachToComponent(PlayerA->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			"BackItemStorageGeneral");

		FTransform socketTransform = PlayerA->GetMesh()->GetSocketTransform("BackItemStorageGeneral");
		HotbarInventory[PrevItemIndex]->ItemData->ItemActor->SetActorTransform(socketTransform);
	}
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
/// do i need this simply check if we have a weapon equipped in current slot
/// Called from ABP
/// TODO: Optimize this, it shouldn't be every frame. Only called when we load inventory or finish swapping weapons immediately
/// </summary>
/// <returns></returns>
bool USPlayerInventoryComponent::HasWeaponEquippedCheck()
{
	bool weaponEquipped = false;
	if(EquippedSlotIndex < HotbarInventory.Num() &&  EquippedSlotIndex >= 0) {
		if (HotbarInventory[EquippedSlotIndex] != nullptr && HotbarInventory[EquippedSlotIndex]->IsWeapon)
		{
			weaponEquipped = true;
		}
	}
	return weaponEquipped;
}

void USPlayerInventoryComponent::SetEquippedItem(UItemBase* NewItem)
{
	if(NewItem != nullptr) {
		EquippedItem = NewItem; 
		EquipedWeaponFromInventory = nullptr;
		EquipedWeaponFromInventory = Cast<USBaseWeapon>(EquippedItem);
		//TODO: should the bool check occur here?
		if(HotbarInventory[EquippedSlotIndex]) {
			bHasWeaponEquipped = HotbarInventory[EquippedSlotIndex]->IsWeapon;
		}
		else 
			bHasWeaponEquipped = false;
	}
	else {
		EquippedItem = nullptr;
		EquipedWeaponFromInventory = nullptr;
		bHasWeaponEquipped = false;
	}
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
/// Utility function
/// Weapons can have multiple meshes, will standardize the various parts of the weapon to make finding certain parts like mag
/// or base weapon model easier. For example each weapon should have a center we use for storage on the back of player
/// </summary>
/// <param name="ActorToCheck"></param>
/// <param name="CompName"></param>
/// <returns></returns>
UStaticMeshComponent* USPlayerInventoryComponent::GetStaticMeshCompByName(AActor* ActorToCheck, FName CompName)
{
	TArray<UStaticMeshComponent*>Meshes;

	//get all meshes from actor and then check which has the proper name we want
	ActorToCheck->GetComponents<UStaticMeshComponent>(Meshes);
	for (UStaticMeshComponent* subSM : Meshes)
	{
		if (subSM && subSM->GetFName() == CompName)
		{
			return subSM;
		}
	}

	return nullptr;
}
