// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.


#include "Actions/SAction_WeaponReload.h"
#include "Player/SCharacter.h"
#include "AI/SAICharacter.h"
#include "Weapons/USFirearmWeapon.h"

void USAction_WeaponReload::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	//if instigator is type player
	ASCharacter* Character = Cast<ASCharacter>(Instigator);
	ASAICharacter* AI = Cast<ASAICharacter>(Instigator);
	UE_LOG(LogTemp, Log, TEXT("In start action implementation of reload"));

	if (Character)
	{
		InventoryComponent = Character->GetPlayerInventoryComp();
	}
	else if (AI) { //AI Inventory
		InventoryComponent = AI->GetInventoryComp();
	}
	else {
		//throw error?
		UE_LOG(LogTemp, Error, TEXT("Failed Assignment of InventoryComp. Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
	}

	//if weapon, get weapon stats and run weapon action, else run consumable action
	UInventorySlot* ItemEquipped = InventoryComponent->GetEquippedSlot();
	if (ItemEquipped->IsWeapon) {

		EquipedWeaponFromInventory = Cast<USBaseWeapon>(ItemEquipped->ItemData);
		WeaponReloadAnimAction = EquipedWeaponFromInventory->ReloadAnim;
		ReloadAnimDelayAction = EquipedWeaponFromInventory->ReloadAnimDelay;

		ensure(EquipedWeaponFromInventory);
		//reload time stat?

		if (Character)
		{
			//set tag
			//Animation should play from the player's animBlueprint
			//how to get different animations to be sent and read in the anim blueprint?
			//something should get set here right?
			//as a note: the anim blueprint REACTS to the actions. Actions DECIDES when the reload starts

			//Not in locomotive, its upper botty and locomotive is purely movement
			
			//trigger the animBlueprint
			// montage is sent to ABP?
			// animInstance is the ABP i think
			// the AnimBP knows based on what tags in its state machine, so the idea is that we start this, its running in our animinstance
			// and based on the state on if reloading that is read, the other animations react accordingly?
			Character->PlayAnimMontage(WeaponReloadAnimAction);
			

			if (Character->HasAuthority()) {
				FTimerHandle TimerHandle_AttackDelay;
				FTimerDelegate Delegate;
				//unless there is a 'warm up' animation that has to run before we can fire the weapon or make the attack, this will most likely be near 0
				Delegate.BindUFunction(this, "ReloadDelay_Elasped", Character);

				//when timer finishes, spawn projectile
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, ReloadAnimDelayAction, false);
			}
		}
	}
}

/// <summary>
/// after reload animation and time is over, then can remove tag?
/// </summary>
/// <param name="InstigatorCharacter"></param>
void USAction_WeaponReload::ReloadDelay_Elasped(ACharacter* InstigatorCharacter)
{
	EquipedWeaponFromInventory->PerformReloadStats();
	StopAction(InstigatorCharacter);
}
