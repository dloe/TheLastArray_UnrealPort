// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "ActorComponents/SBaseInventoryComponent.h"
#include "SAction_WeaponAttack.generated.h"

/**
 * 
 */
UCLASS()
class USAction_WeaponAttack : public USAction
{
	GENERATED_BODY()

	public:
	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	//UFUNCTION(BlueprintNativeEvent, Category = "Action")
	virtual void StartAction_Implementation(AActor* Instigator) override;

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	UFUNCTION()
	void SetInventoryComp(USBaseInventoryComponent* InventoryCompRef) { InventoryComponent = InventoryCompRef; };
	
	private:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	//delay from animation to end action
	UFUNCTION()
	void AttackAnimDelay_Elasped(ACharacter* InstigatorCharacter);

	//delay overall attack
	UFUNCTION()
	void AttackDelay_Elasped(ACharacter* InstigatorCharacter);

	UFUNCTION()
	void EjectCasing(ACharacter* InstigatorCharacter);

	UFUNCTION()
	void FireProjectile(ACharacter* InstigatorCharacter);

	//might not need this anymore because of new custom notify
	UFUNCTION()
	void OnFireMontageFinished(ACharacter* InstigatorCharacter, UAnimMontage* PostFireMon, bool bInterrupted);

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	//vars tied directly to actions, only set when action starts and those props maintained until the aciton ends
	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	USBaseWeapon* EquipedWeaponFromInventory;

	UPROPERTY(EditAnywhere,  Category = "Equipped Weapon Properties - Overriden by equipped item")
	UAnimMontage* EquipedWeaponAttackAnimAction;

	UPROPERTY(EditAnywhere,  Category = "Equipped Weapon Properties - Overriden by equipped item")
	UAnimMontage* EquippedWeaponPostAttackAnimation;

	UPROPERTY(EditAnywhere,  Category = "Equipped Weapon Properties - Overriden by equipped item")
	UParticleSystem* EquipedWeaponCastingEffectsAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	FName EquipedSpawnSocketNameAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	float EquipedAttacAnimDelayAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	float EquipedDelayAttack;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	TSubclassOf<AActor> EquipedWeaponProjectileSubclassAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	UStaticMeshComponent* EquipedWeaponStaticMesh;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	TSubclassOf<AAEjectedBulletCasing> EquipedWeaponCasing;

	//runs attack procedure for currently equipped weapon in actor's inventory
	//this should be assigned when we a reference to this class is established to the controller class?
	UPROPERTY(EditAnywhere, Category = "Inventory")
	USBaseInventoryComponent* InventoryComponent;
};
