// Copyright (c) 2026 Dylan. Personal Game Project. 
// 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "Actions/SAction_SwapItem.h"
#include "Player/SCharacter.h"
#include "Actions/SActionComponent.h"

/// <summary>
/// Hotbar referenced item is the one we are swapping TO
/// ItemEquipped is the currently equipped item
/// </summary>
/// <param name="Instigator"></param>
void USAction_SwapItem::StartAction_Implementation(AActor* Instigator)
{
	Character = Cast<ASCharacter>(Instigator);

	if (!Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartAction_Implementation is not running off character %s"), *Instigator->GetName());
	}

	InventoryComponent = Character->GetPlayerInventoryComp();
	InventoryComponent->bCanRunEquipBehavior = true; //dont want dub calls to anim notifies for equip

	ItemEquipped = InventoryComponent->GetEquippedItem(); //item we currently have equipped
	//TODO: use consts more
	const int WeaponSlotIndex = InventoryComponent->CurrentHotbarSlot(); //which index are we currently on

	TargetIndex = InventoryComponent->HotbarToSwapTo; //index we are swapping to

	//first check if we need to start the action at all
	if(!InventoryComponent->HotbarInventory[TargetIndex])
		return;

	UItemBase* TargetItemData = InventoryComponent->HotbarInventory[TargetIndex]->ItemData;

	//skip logic if no actual change is occurring, if prev item and new item are empty or the same index being swapped to
	if ((ItemEquipped == nullptr && TargetItemData == nullptr) || (WeaponSlotIndex == TargetIndex))
	{
		//nothing changes really
		return;
	}

	//standard start action start
	Super::StartAction_Implementation(Instigator);

	float EquipTime = 0.0f;
	float DeEquipTime = 0.0f;

	EWeaponType PrevWeaponType = EWeaponType::ENone;
	EWeaponType NewWeaponType = EWeaponType::ENone;
	if(ItemEquipped && ItemEquipped->ItemType == EItemType::EWeapon) {
		USBaseWeapon* PrevWeapon = Cast< USBaseWeapon>(ItemEquipped);
		PrevWeaponType = PrevWeapon->WeaponType;
	}
	if (TargetItemData && TargetItemData->ItemType == EItemType::EWeapon) {
		USBaseWeapon* NewWeapon = Cast< USBaseWeapon>(TargetItemData);
		NewWeaponType = NewWeapon->WeaponType;
	}

	//three possible cases for swapping items
	//case 1 Item to item swappage
	if (ItemEquipped && TargetItemData)
	{
		//UE_LOG(LogTemp, Log, TEXT("item to item swap"));
		DeEquipTime = ItemEquipped->DeEquipMont->GetPlayLength();
		EquipTime = TargetItemData->DeEquipMont->GetPlayLength();
		EquippedItemToItemEvent(
			ItemEquipped->ItemType, //prev
			TargetItemData->ItemType,
			ItemEquipped->DeEquipMont, //prev
			TargetItemData->EquipMont,
			PrevWeaponType, NewWeaponType
		);

	}
	//case 2 item to none swappage
	else if (ItemEquipped && TargetItemData == nullptr)
	{
		//UE_LOG(LogTemp, Log, TEXT("item to None swap"));
		DeEquipTime = ItemEquipped->DeEquipMont->GetPlayLength();
		EquippedItemToNoneEvent(
			ItemEquipped->ItemType, //prev
			ItemEquipped->DeEquipMont, //prev
			PrevWeaponType
		);
	}
	//case 3 none to item swappage
	else if (ItemEquipped == nullptr && TargetItemData)
	{
		//UE_LOG(LogTemp, Log, TEXT("None to item swap"));
		EquipTime = TargetItemData->DeEquipMont->GetPlayLength();
		EquippedNoneToItemEvent(
			TargetItemData->ItemType,
			TargetItemData->EquipMont,
			NewWeaponType
		);
	}

	//update inventory status
	InventoryComponent->SetHotbarSlot(TargetIndex);

	//save previous type for next time
	if (ItemEquipped)
		PrevEquippedItemType = ItemEquipped->ItemType;
	InventoryComponent->SetEquippedItem(TargetItemData);

	//wait for the amount of time of the 2 sequences
	FTimerHandle TimerHandle_SwapDelay;
	FTimerDelegate DelegateSwapDelay;
	DelegateSwapDelay.BindUFunction(this, "SwapDelay_Elasped", Character);

	const float SwapDelayTotal = EquipTime + DeEquipTime + SwapCooldown; //add a sec delay after animations end
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwapDelay, DelegateSwapDelay, SwapDelayTotal, false);
}

void USAction_SwapItem::SwapDelay_Elasped(ACharacter* InstigatorCharacter)
{
	if (ActionComp != nullptr)
		ActionComp->StopActionByName(Character, "SwapItems");
}
