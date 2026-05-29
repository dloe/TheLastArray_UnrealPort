// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "Animations/AnimNotify_DeEquipItem.h"
#include "Player/SCharacter.h"

/// <summary>
/// Behavior from when the equip / dequip notifies dictate when things like the weapons spawning or despawning occur
/// </summary>
/// <param name="MeshComp"></param>
/// <param name="Animation"></param>
void UAnimNotify_DeEquipItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Instigator = MeshComp->GetOwner();
	if (Instigator)
	{
		//call characters player inventory equip item behavior to trigger
		if (ASCharacter* Player = Cast<ASCharacter>(Instigator))
		{
			USPlayerInventoryComponent* Inventory = Player->GetPlayerInventoryComp();
			Inventory->DeEquipItemBehavior();
		}
	}
	
}