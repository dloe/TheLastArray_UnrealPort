// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.

#include "Weapons/SBaseWeapon.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values
USBaseWeapon::USBaseWeapon()
{


}

/// <summary>
/// if player hits attack and we have ammo spend ammo and attack
/// 
/// spawn projectile? For base class we don't have magazine so pull from straight ammo reserves
/// </summary>
void USBaseWeapon::Attack()
{
	ensure(AmmoCount > 0);

	//think of these attacks as a provider of action
	//like each weapon has an action of firing tied to it
	//for example this base class could have the USAction_ProjectileAttack tied to it
	//so when the player does a primary attacks we go through the inventory, to the selected weapon for 
	//and runs the selected action (StartAction)

	//ideal architecture: player hits button -> runs primaryattack function
	// -> start action("PrimaryAttack") -> ActionComponent functiuon for primary attack
	// -> currently equiped weapon has attack function

	//action should look up weapon not the other way around

	//weapon should own the weapon specific animation

	//before we have the animation live in the Projectile Attack class (action child class)

	//action comp shouldn't care what weapon is equipped, doesn't care why its firing
	//player char doesn't need to know anything besides i have a weapon equipped


	//note: want to be able to add weapons without touching the action comp
	// can add new actions without touching the weapon system
	// make sure AI can use same weapons
	// ideally want weapons to be tuned separately 
	//want animations for crazy ass weapons to also be stored independently 
	//the projectile attack fireball action class handles everything, may want to split it up

	//make new USAction class that incorporates currently equipped weapon
	//weapon will now handle ProjectileClass, SpawnSocketName (where projectile spawns - since it
	// all depends on gun or weapon model), AttacAnimDelay, AttackAnim, AttackEffects

	//should have a reference to player inventory more specifically the equipped weapon
	//should be able to not just run the weapon firing sequence on ACharacter but also AICharacter
}

/// <summary>
/// if player hits attack button and we don't have any bullets in mag, run reload (and if we have more ammo to reload
/// </summary>
void USBaseWeapon::Reload()
{
	
}

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

