// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// 
// Unauthorized use, distribution, or modification is not permitted.

#include "Weapons/SBaseWeapon.h"
#include "Items/UItemBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"


//think of these attacks as a provider of action
// like each weapon has an action of firing tied to it
//for example this base class could have the USAction_ProjectileAttack tied to it
//so when the player does a primary attacks we go through the inventory, to the selected weapon for 
//and runs the selected action (StartAction)

//ideal architecture: player hits button -> runs primaryattack function
// -> start action("PrimaryAttack") -> ActionComponent function for primary attack
// -> currently equiped weapon has attack function

//action should look up weapon not the other way around
//weapon should own the weapon specific animation

//action comp shouldn't care what weapon is equipped, doesn't care why its firing
//player char doesn't need to know anything besides i have a weapon equipped

//note: want to be able to add weapons without touching the action comp
// can add new actions without touching the weapon system
// make sure AI can use same weapons
// ideally want weapons to be tuned separately 
//want animations for crazy ass weapons to also be stored independently 
//the projectile attack fireball action class handles everything, may want to split it up

//should be able to not just run the weapon firing sequence on ACharacter but also AICharacter

/// <summary>
/// When player swaps weapon from manager class, maybe a specific weapon behavior occurs? sound, reload sound could play, etc
/// </summary>
void USBaseWeapon::OnEquip()
{
	
}

/// <summary>
/// Sound plays when weapon is put away but animation should play from player animation class?
/// </summary>
void USBaseWeapon::Unequip()
{
	
}

/// <summary>
/// Check if we have ammo to reload and return
/// Magazine status is less than max mag size
/// Ammo to even reload
/// </summary>
/// <returns></returns>
bool USBaseWeapon::CanBeReloaded()
{
	return false;
}


/// <summary>
/// refill the mag after reload animation finishes
/// </summary>
/// <returns></returns>
void USBaseWeapon::PerformReloadStats()
{
	UE_LOG(LogTemp, Error, TEXT("Not reloadable since base class. Instigator issue... [Class: %s]"), *GetNameSafe(this));
	return;
}

bool USBaseWeapon::CanAttackWithWeapon()
{
	return true;
}

/// <summary>
/// Moved the attack behavior from the action to be called directly here
/// </summary>
/// <param name="Instigator"></param>
void USBaseWeapon::PerformAttack(AActor* Instigator, USAction_WeaponAttack* OwningAttackAction)
{
	PlayAnimation(Instigator, AttackAnim);

	APawn* InstigatorPawn = Cast<APawn>(Instigator);
	if(InstigatorPawn) {
		InstigatorPawn->MakeNoise(
			1.0f,
			InstigatorPawn,
			InstigatorPawn->GetActorLocation(),
			0.0f,
			TEXT("Hand Projectile Fire")
		);
	}
}



void USBaseWeapon::PerformReload(AActor* Instigator, USAction_WeaponReload* OwningReloadAction)
{
	ACharacter* InstigatorCharacter = Cast< ACharacter>(Instigator);
	if (!InstigatorCharacter) //not character?
		return;

	UAnimInstance* AnimInstance = InstigatorCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !ReloadAnim)
	{
		UE_LOG(LogTemp, Error, TEXT("PerformReloade: Cant get animinstance. Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
		return;
	}

	InstigatorCharacter->PlayAnimMontage(ReloadAnim);
}

/// <summary>
/// Ideally the ai should be able to play the attack montage as well. Make sure this works
/// </summary>
/// <param name="Instigator"></param>
void USBaseWeapon::PlayAnimation(AActor* Instigator, UAnimMontage* AnimToPlay)
{
	ACharacter* InstigatorCharacter = Cast< ACharacter>(Instigator);
	if(!InstigatorCharacter) //not character?
		return;

	UAnimInstance* AnimInstance = InstigatorCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant get animinstance. Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
		return;
	}
	if (!AnimToPlay)
	{
		UE_LOG(LogTemp, Error, TEXT("No AnimToPlay for action. Possible Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
		return;
	}

	InstigatorCharacter->PlayAnimMontage(AnimToPlay);
}

/// <summary>
/// This solves the dependency issue!!
/// </summary>
/// <returns></returns>
EWeaponType USBaseWeapon::GetWeaponType_Implementation() const
{
	return WeaponType;
}

/// <summary>
/// Is a weapon, therefore is equippable
/// </summary>
/// <returns></returns>
bool USBaseWeapon::IsEquippable_Implementation_Implementation() const
{
	return true;
}

