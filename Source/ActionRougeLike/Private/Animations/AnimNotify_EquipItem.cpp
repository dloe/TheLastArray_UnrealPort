// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "Animations/AnimNotify_EquipItem.h"
#include "Player/SCharacter.h"



/// <summary>
/// Behavior from when the equip / dequip notifies dictate when things like the weapons spawning or despawning occur
/// 
/// Ideally the notify shouldn't know about the action behavior themselves right?
/// </summary>
/// <param name="MeshComp"></param>
/// <param name="Animation"></param>
void UAnimNotify_EquipItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Instigator = MeshComp->GetOwner();
	if (Instigator)
	{
		//call characters player inventory equip item behavior to trigger
		if (ASCharacter* Player = Cast<ASCharacter>(Instigator))
		{
			USPlayerInventoryComponent* Inventory = Player->GetPlayerInventoryComp();
			if(Inventory->bCanRunEquipBehavior) {
				Inventory->bCanRunEquipBehavior = false;
				Inventory->EquipItemBehavior();
				
			}
		}
	}
}