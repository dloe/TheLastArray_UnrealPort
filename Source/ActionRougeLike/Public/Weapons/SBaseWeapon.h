// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectiles/SProjectileBase.h"
#include "Items/UItemBase.h"
#include "SBaseWeapon.generated.h"

class UAnimMontage;
class UParticleSystem;

UCLASS(Blueprintable, DefaultToInstanced)
class ACTIONROUGELIKE_API USBaseWeapon : public UItemBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	USBaseWeapon();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------
	//how bad do i want to copy warframe's damage system? 0.0
	//REMINDER: This needs to be tied to action system i implemented in player
	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	//IS THIS IN USE RN? since we have actions for attack and reload
	//UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	//virtual void Attack();

	//UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	//virtual void Reload();

	UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	virtual void OnEquip();

	UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	virtual void Unequip();

	UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	virtual bool CanBeReloaded();


	//@TODO: This should be tied to bullet or projectile
	//UFUNCTION(BlueprintCallable, Category = "Weapon Behavior")
	//virtual void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

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

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int CurrentMagazineSize;

	UPROPERTY(EditAnywhere, Category = "Weapon Stats")
	int StandardMagazineSized;

	//@TODO: maybe make enum in future?
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FName WeaponClassification;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FName WeaponDescription;

	//UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	//FName WeaponSocketName = "Muzzle_01";

	//skeletal mesh / static mesh comp?


	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<AActor> WeaponProjectile;

		//may move into child class for the projectile based weapons
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Reload")
	TSubclassOf<AActor> MagazineActor;

	UPROPERTY(EditAnywhere, Category = "Firing")
	TSubclassOf<AActor> EjectedCasingActor;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttacAnimDelay = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* PostATtackIronSights;

	UPROPERTY(EditAnywhere, Category = "Reload")
	UAnimMontage* ReloadAnim;
	UPROPERTY(VisibleAnywhere, Category = "Reload")
	float ReloadAnimDelay = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* CastingEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Properties")
	FName WeaponMuzzleSocketName;

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------


	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
	


};
