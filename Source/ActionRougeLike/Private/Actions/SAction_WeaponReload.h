// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "ActorComponents/SBaseInventoryComponent.h"
#include "SAction_WeaponReload.generated.h"

/**
 * 
 */
UCLASS()
class USAction_WeaponReload : public USAction
{
	GENERATED_BODY()
	
	public:
	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	//UFUNCTION(BlueprintNativeEvent, Category = "Action")
	virtual void StartAction_Implementation(AActor* Instigator) override;

	UFUNCTION()
	void SetInventoryComp(USBaseInventoryComponent* InventoryCompRef) { InventoryComponent = InventoryCompRef; };

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	USBaseWeapon* EquipedWeaponFromInventory;

	//TBD: @TODO: reload audio from baseweapon class?

		private:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	UFUNCTION()
	void ReloadDelay_Elasped(ACharacter* InstigatorCharacter);

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
	UPROPERTY(EditAnywhere,  Category = "Equipped Weapon Properties")
	UAnimMontage* WeaponReloadAnimAction;

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	float ReloadAnimDelayAction;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	USBaseInventoryComponent* InventoryComponent;
};
