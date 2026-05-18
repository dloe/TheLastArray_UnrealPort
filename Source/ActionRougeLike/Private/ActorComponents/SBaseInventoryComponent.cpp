// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "ActorComponents/SBaseInventoryComponent.h"
#include "Player/SCharacter.h"
#include "Components/SkinnedMeshComponent.h"

// Sets default values for this component's properties
USBaseInventoryComponent::USBaseInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USBaseInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

/// <summary>
/// Inside of inventory movement only (no hotbar since it only exists in player)
/// 
/// move one index of an array to another (modifier for array of items directly)
/// </summary>
/// <param name="IndexA"></param>
/// <param name="IndexB"></param>
void USBaseInventoryComponent::SwapInventorySlots(int IndexA, int IndexB)
{
	ensure(IndexA < Inventory.Num() && IndexA >= 0);
	ensure(IndexB < Inventory.Num() && IndexB >= 0);
	UInventorySlot* tempCopy = Inventory[IndexA];
	//ensure(!tempCopy->isEmptySlot()); //might not need this check

	Inventory[IndexA] = Inventory[IndexB];
	Inventory[IndexB] = tempCopy;
}

// Called every frame
void USBaseInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/// <summary>
/// Utility function to get the currently equipped weapon
/// 
/// NOTE: Player inventory will pull from hotbar, AI will pull from elsewhere
/// Maybe specific weapon slot on AI? AI wont have the same type of inventory as player (just usable weapon, drops on kill, etc)
/// </summary>
/// <returns></returns>
UInventorySlot* USBaseInventoryComponent::GetEquippedSlot()
{
	//THIS WILL BE SWAPPED WITH HOTBAR ARRAY BTW
	//ensure(EquippedSlotIndex < Inventory.Num() && EquippedSlotIndex >= 0);

	if (EquippedSlotIndex < Inventory.Num() && EquippedSlotIndex >= 0)
		return Inventory[EquippedSlotIndex];
	else
		return nullptr;

}

/// <summary>
/// When an actor dies with inventory, should be some behavior for dropping
/// 
/// Detach equipped item to fall on floor
/// Will override to account for drop table
/// Maybe player has slightly different behavior we override?
/// </summary>
void USBaseInventoryComponent::OnDeathInventoryDrop()
{
	
	//Detach equipped item to fall on ground
	
}

USBaseWeapon* USBaseInventoryComponent::GetCurrentWeaponInfo()
{
	UInventorySlot* CurrentSlot = GetEquippedSlot();
	if(CurrentSlot)
		return CurrentSlot->GetWeaponData();
	else
		return nullptr;
}

/// <summary>
/// Using component space instead of bone space for weapon left hand ik socket
/// otherwise causes AO related drifting when i look up and down. Left and right problably too but i used the 
/// rotate in place to hid it.
/// </summary>
/// <param name="Instigator"></param>
/// <returns></returns>
FTransform USBaseInventoryComponent::GetLeftHandTransform(ASCharacter* Instigator)
{
	FTransform result = FTransform::Identity;
	FVector BoneLoc;
	FRotator BoneRot;

	USBaseWeapon* CurrentWeapon = GetCurrentWeaponInfo();
	if (CurrentWeapon && CurrentWeapon->ItemActor && Instigator)
	{
		UStaticMeshComponent* SM = CurrentWeapon->GetItemStaticMesh();
		//get socket in world space
		FTransform FLeftSocketTrans = SM->GetSocketTransform("LeftHandSocket", RTS_World);

		//transform into bone space of our player character (the space between the item we pull this from and the player are different)
		//get ref to player
		UStaticMeshComponent* PlayerSM = Instigator->FindComponentByClass<UStaticMeshComponent>();
		USkeletalMeshComponent* Mesh = Instigator->GetMesh();

		// component space
		FTransform LeftHandCS = FLeftSocketTrans.GetRelativeTransform(Mesh->GetComponentTransform());

		//bone space attempt, TODO: will remove after more testing
		//the big part here, put our sockets transforms spacing in relation to hand_r
		Mesh->TransformToBoneSpace(
			"hand_l", //hand_r
			FLeftSocketTrans.GetLocation(),
			FLeftSocketTrans.Rotator(),
			BoneLoc, 
			BoneRot
		);
		DrawDebugSphere(GetWorld(), FLeftSocketTrans.GetLocation(), 4.0f, 6, FColor::Red, false, 0.0f);

		//cs
		result = FTransform(LeftHandCS.GetRotation(), LeftHandCS.GetLocation(), FVector(1,1,1));
		//bone space - TODO: keep for just just in case
		//result = FTransform(BoneRot, BoneLoc, FVector(1, 1, 1));
	}
	return result;
}

/// <summary>
/// Takes weapon and ads to overall inventory
/// 
/// maybe add to base inventory class instead of player? what difference would this be than AI one?
/// 
/// This could be optimized? what if its a bigger inventory?
/// </summary>
/// <param name="Weapon"></param>
/// <returns></returns>
bool USBaseInventoryComponent::AddItemToInventory(UItemBase* ItemToAdd)
{
	bool AssignedItemToInventory = false;

	for (int index = 0; index < Inventory.Num(); index++)//UInventorySlot* Slot : Inventory)
	{
		//find first one that is null, assign and return and return
		if (Inventory[index]->isEmptySlot())
		{
			AssignedItemToInventory = true;
			Inventory[index]->AssignItem(ItemToAdd);
			//@TODO: what about the other properties of inventory slot? 
			//assign item handles it but may be unoptimal...


		}
	}

	return AssignedItemToInventory;
}

/// <summary>
/// On begin play, this is called from Character script
/// if we have loaded info, set that up here. Else use a stock inventory
/// </summary>
void USBaseInventoryComponent::LoadInventory(AActor* Instigator)
{
	//default to base inventory size, but if we have upgrades that will be passed in
	Inventory.SetNum(BaseInventorySize);

	//default loadout on start? we presave and then load in?
}

