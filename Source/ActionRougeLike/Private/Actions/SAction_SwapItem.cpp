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
	if (Character)
	{
		InventoryComponent = Character->GetPlayerInventoryComp();
		SMComp = Character->FindComponentByClass<UStaticMeshComponent>(); //TODO: Dont need anymore - verify in bp
	}
	InventoryComponent->bCanRunEquipBehavior = true; //dont want dub calls to anim notifies for equip
	ItemEquipped = InventoryComponent->GetEquippedItem(); //item we currently have equipped
	int WeaponSlotIndex = InventoryComponent->CurrentHotbarSlot(); //which index are we currently on
	UE_LOG(LogTemp, Log, TEXT("Current Slot Index %d"), WeaponSlotIndex);
	HotBarIndexSwappingTo = InventoryComponent->HotbarToSwapTo; //index we are swapping to

	//first check if we need to start the action at all
	//skip logic if no actual change is occurring, if prev item and new item are empty or the same index being swapped to
	if ((ItemEquipped == nullptr && InventoryComponent->HotbarInventory[HotBarIndexSwappingTo]->ItemData == nullptr) ||
		(WeaponSlotIndex == HotBarIndexSwappingTo))// && InventoryComponent->HotbarInventory[HotBarIndexSwappingTo]->ItemData == nullptr))
	{
		//nothing changes really
		return;
	}

	//standard start action routine
	Super::StartAction_Implementation(Instigator);

	float seqEquipTime = 0.0f;
	float seqDeEquipTime = 0.0f;
	//if both are real slots, can use all 4 delegate parameters
	if (ItemEquipped != nullptr)
	{
		seqDeEquipTime = ItemEquipped->DeEquipMontage->GetPlayLength();
		if(InventoryComponent->HotbarInventory[HotBarIndexSwappingTo]->ItemData != nullptr) { //both are not null, full swap
			EquippedItemToItemEvent(ItemEquipped->ItemType, InventoryComponent->HotbarInventory[WeaponSlotIndex]->ItemData->ItemType, 
			ItemEquipped->DeEquipMontage,InventoryComponent->HotbarInventory[WeaponSlotIndex]->ItemData->EquipMontage);
			seqEquipTime = InventoryComponent->HotbarInventory[WeaponSlotIndex]->ItemData->EquipMontage->GetPlayLength();
		}
		else { //index we swapping to null, dont need to have the equip behavior, only deequip
			EquippedItemToNoneEvent(ItemEquipped->ItemType, ItemEquipped->DeEquipMontage);

		}
	} else { //going from no item equipped to a real item (prev is none)
		EquippedNoneToItemEvent(InventoryComponent->HotbarInventory[HotBarIndexSwappingTo]->ItemData->ItemType,
		InventoryComponent->HotbarInventory[HotBarIndexSwappingTo]->ItemData->EquipMontage);

		seqEquipTime = InventoryComponent->HotbarInventory[HotBarIndexSwappingTo]->ItemData->EquipMontage->GetPlayLength();
	}
	InventoryComponent->SetHotbarSlot(HotBarIndexSwappingTo);

	//save previous type for next time
	if (ItemEquipped)
		PrevEquippedItemType = ItemEquipped->ItemType;
	InventoryComponent->SetEquippedItem(InventoryComponent->HotbarInventory[HotBarIndexSwappingTo]->ItemData);

	//wait for the amount of time of the 2 sequences
	FTimerHandle TimerHandle_SwapDelay;
	FTimerDelegate DelegateSwapDelay;
	float SwapDelayTotal = seqEquipTime + seqDeEquipTime + 1.00f; //add a sec delay after animations end
	DelegateSwapDelay.BindUFunction(this, "SwapDelay_Elasped", Character);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SwapDelay, DelegateSwapDelay, SwapDelayTotal, false);
}

void USAction_SwapItem::UNUSED_EquipItemBehavior(AActor* Instigator)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = Character;

	//@TODO: socket might change based on what item, take that into account (if weapon vs equippable item)
	FTransform socketTransform = Character->GetMesh()->GetSocketTransform(ItemEquipped->HandSocketName);

	ItemEquipped->ItemActor = GetWorld()->SpawnActor<AActor>(ItemEquipped->ItemActorSubclass, socketTransform, SpawnParams);

	//attach to socket
	ItemEquipped->ItemActor->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		ItemEquipped->HandSocketName);

	//can turn off swap weapon action
	if(ActionComp && ActionComp->CheckActionName("SwapItems"))
		Character->GetActionComp()->StopActionByName(Character, "SwapItems");
}

void USAction_SwapItem::OLDDeEquipItemBehavior()
{
	InventoryComponent->RemoveItemVisibilitiyByIndex(PrevItemIndex);
}

void USAction_SwapItem::SwapDelay_Elasped(ACharacter* InstigatorCharacter)
{
	if (ActionComp != nullptr)
		ActionComp->StopActionByName(Character, "SwapItems");
}
