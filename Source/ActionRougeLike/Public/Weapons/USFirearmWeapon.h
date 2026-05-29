// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/SBaseWeapon.h"
#include "USFirearmWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USFirearmWeapon : public USBaseWeapon
{
	GENERATED_BODY()
	
public:

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	virtual bool CanBeReloaded();

	virtual void PerformReloadStats();

	virtual void PerformAttack(AActor* Instigator, USAction_WeaponAttack* OwningAttackAction);

	virtual void PerformReload(AActor* Instigator, USAction_WeaponReload* OwningReloadAction);

	virtual bool CanAttackWithWeapon();

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int CurrentMagazineSize;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Stats")
	int StandardMagazineSized;

	//audio
	UPROPERTY(EditAnywhere, Category = "Weapon Audio")
	USoundBase* ReloadSound;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<AActor> WeaponProjectile;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Reload")
	TSubclassOf<AActor> MagazineActor;

	UPROPERTY(EditAnywhere, Category = "Firing")
	TSubclassOf<AAEjectedBulletCasing> EjectedCasingActor;

	//UPROPERTY(EditAnywhere, Category = "Reload")
	//UAnimMontage* ReloadAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	FName WeaponMuzzleSocketName;

protected:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	void SpawnProjectile(AActor* Instigator);

	//UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	//void PlayAttackAnimation(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	void SpawnCasing(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Attack Behavior")
	void PlayMuzzleFx(AActor* Instigator);

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

};
