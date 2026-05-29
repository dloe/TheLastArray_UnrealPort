// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.


#include "Actions/SAction_WeaponReload.h"
#include "Player/SCharacter.h"
#include "AI/SAICharacter.h"
#include "Weapons/USFirearmWeapon.h"

void USAction_WeaponReload::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USBaseInventoryComponent* InventoryComponent = ResolveInventory(Instigator);

	if (!InventoryComponent)
	{
		//cant perform action if no inventory
		StopAction(Instigator);
		return;
	}

	//if weapon, get weapon stats and run weapon action, else run consumable action
	UInventorySlot* EquippedSlot = InventoryComponent->GetEquippedSlot();

	//can we reload?
	if (!EquippedSlot || !EquippedSlot->IsWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponReloadAction: Failed attacking of slot. Instigator issue... [Class: %s]"), *GetNameSafe(InventoryComponent));
		StopAction(Instigator);
		return;
	}

	//get current weapon
	CurrentEquippedWeapon = Cast<USBaseWeapon>(EquippedSlot->ItemData);
	if (!CurrentEquippedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponReloadAction: Failed to get weapon. Instigator issue... [Class: %s]"), *GetNameSafe(EquippedSlot->ItemData));
		StopAction(Instigator);
		return;
	}
	//as a note: the anim blueprint REACTS to the actions. Actions DECIDES when the reload starts
	//Not in locomotive, its upper botty and locomotive is purely movement
	CurrentEquippedWeapon->PerformReload(Instigator, this);

}

/// <summary>
/// after reload animation and time is over, then can remove tag?
/// </summary>
/// <param name="InstigatorCharacter"></param>
void USAction_WeaponReload::ReloadDelay_Elasped(ACharacter* InstigatorCharacter)
{
	CurrentEquippedWeapon->PerformReloadStats();
	StopAction(InstigatorCharacter);
}

/// <summary>
/// Inventory lookup
/// </summary>
/// <param name="InstigatingActor"></param>
/// <returns></returns>
USBaseInventoryComponent* USAction_WeaponReload::ResolveInventory(AActor* InstigatingActor) const
{
	ASCharacter* Character = Cast<ASCharacter>(InstigatingActor);
	if (Character)
	{
		return Character->GetPlayerInventoryComp();
	}

	ASAICharacter* AI = Cast<ASAICharacter>(InstigatingActor);
	if (AI)
	{
		return AI->GetInventoryComp();
	}

	//if we get this far, panic
	UE_LOG(LogTemp, Error, TEXT("WeaponReloadAction: Failed Assignment of InventoryComp. Instigator issue... [Class: %s]"), *GetNameSafe(InstigatingActor));
	return nullptr;
}
