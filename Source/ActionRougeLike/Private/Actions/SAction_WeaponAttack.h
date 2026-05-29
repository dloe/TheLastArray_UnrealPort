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


class USBaseWeapon; 
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

	UFUNCTION()
	void OnWeaponAttackFinished(AActor* Instigator);

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	

	private:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	
	//delay overall attack
	UFUNCTION()
	void AttackDelay_Elasped(AActor* Instigator);

	UFUNCTION()
	USBaseInventoryComponent* ResolveInventory(AActor* InstigatingActor) const;

	UFUNCTION()
	void StartAttackAfterDelay(AActor* Instigator);

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	//vars tied directly to actions, only set when action starts and those props maintained until the action ends
	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	USBaseWeapon* CurrentEquippedWeapon;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties - Overriden by equipped item")
	UStaticMeshComponent* EquipedWeaponStaticMesh;

};
