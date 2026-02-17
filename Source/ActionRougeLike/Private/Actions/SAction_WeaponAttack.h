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

	UFUNCTION()
	void AttackDelay_Elasped(ACharacter* InstigatorCharacter);

	UFUNCTION()
	void EjectCasing(ACharacter* InstigatorCharacter);

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	//vars tied directly to actions, only set when action starts and those props maintained until the aciton ends
	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	USBaseWeapon* EquipedWeaponFromInventory;

	UPROPERTY(EditAnywhere,  Category = "Equipped Weapon Properties")
	UAnimMontage* EquipedWeaponAttackAnimAction;

	UPROPERTY(EditAnywhere,  Category = "Equipped Weapon Properties")
	UAnimMontage* EquippedWeaponPostAttackAnimation;

	UPROPERTY(EditAnywhere,  Category = "Equipped Weapon Properties")
	UParticleSystem* EquipedWeaponCastingEffectsAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	FName EquipedSpawnSocketNameAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	float EquipedAttacAnimDelayAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	TSubclassOf<AActor> EquipedWeaponProjectileSubclassAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	UStaticMeshComponent* EquipedWeaponStaticMesh;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	TSubclassOf<AActor> EquipedWeaponCasing;

	//runs attack procedure for currently equipped weapon in actor's inventory
	//this should be assigned when we a reference to this class is established to the controller class?
	UPROPERTY(EditAnywhere, Category = "Inventory")
	USBaseInventoryComponent* InventoryComponent;
};
