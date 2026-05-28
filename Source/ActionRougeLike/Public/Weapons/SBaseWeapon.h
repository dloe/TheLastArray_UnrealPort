// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/UItemBase.h"
#include "Projectiles/SProjectileBase.h"
#include "../Private/Items/AEjectedBulletCasing.h"
#include "../Private/Actions/SAction_WeaponAttack.h"
#include "SBaseWeapon.generated.h"

class UAnimMontage;
class UParticleSystem;

UCLASS(Blueprintable, DefaultToInstanced)
class ACTIONROUGELIKE_API USBaseWeapon : public UItemBase
{
	GENERATED_BODY()
	
public:	

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------
	//how bad do i want to copy warframe's damage system? 0.0
	//REMINDER: This needs to be tied to action system i implemented in player

	UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	virtual void OnEquip();

	UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	virtual void Unequip();

	UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	virtual bool CanBeReloaded();

	UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	virtual void PerformReloadStats();

	UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	virtual bool CanAttackWithWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item Properties")
	bool IsEquippable_Implementation() const;

	virtual EWeaponType GetWeaponType_Implementation() const;

	//actions tell weapon to perform weapon specific behavior
	UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	virtual void PerformAttack(AActor* Instigator, USAction_WeaponAttack* OwningAttackAction);

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Stats")
	EWeaponType WeaponType;

	//base damage we start with
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float BaseDamage;

	//headshots automatically apply critical chance
	//what are the chances this gets a critical multiplier to base damage?
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float CriticalChance;

	//How much crit damage would the weapon do if it did get a critical hit?
	//crit damage is a range? or should be a flat bonus damage?
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float CriticalDamage;

	//attacks per second
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	float FireRate;

	//how much can we use the weapon?
	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int TotalAmmoReserves;

	//UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	//int CurrentMagazineSize;

	//UPROPERTY(VisibleAnywhere, Category = "Weapon Stats")
	//int StandardMagazineSized;

	//@TODO: maybe make enum in future?
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FName WeaponClassification;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FName WeaponDescription;

	//skeletal mesh / static mesh comp?

	//audio
	//UPROPERTY(EditAnywhere, Category = "Weapon Audio")
	//USoundBase* ReloadSound;

	//UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	//TSubclassOf<AActor> WeaponProjectile;

		//may move into child class for the projectile based weapons
	//UPROPERTY(EditAnywhere, Category = "Attack")
	//TSubclassOf<AActor> ProjectileClass;

	//UPROPERTY(EditAnywhere, Category = "Reload")
	//TSubclassOf<AActor> MagazineActor;

	//UPROPERTY(EditAnywhere, Category = "Firing")
	//TSubclassOf<AAEjectedBulletCasing> EjectedCasingActor;

	//delaying the start of firing projectile
	UPROPERTY(EditAnywhere, Category = "Attack")
	float StartAttacDelay = 0.f;

	//the delay from the animation to end the attack action (so different from AttacDelay)
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttacAnimDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Reload")
	UAnimMontage* ReloadAnim;

	UPROPERTY(EditAnywhere, Category = "Reload")
	float ReloadAnimDelay = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* CastingEffects;

	//UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	//FName WeaponMuzzleSocketName;


protected:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	/*UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	void OLD_SpawnProjectile(AActor* Instigator);*/

	UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	void PlayAttackAnimation(AActor* Instigator);

	/*UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	void OLD_SpawnCasing(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	void OLD_PlayMuzzleFx(AActor* Instigator);*/

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
	


};
