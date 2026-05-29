// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "ActorComponents/SBaseInventoryComponent.h"
#include "SAction_WeaponReload.generated.h"

class USBaseWeapon;
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

	private:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	UFUNCTION()
	void ReloadDelay_Elasped(ACharacter* InstigatorCharacter);

	UFUNCTION()
	USBaseInventoryComponent* ResolveInventory(AActor* InstigatingActor) const;

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Equipped Weapon Properties")
	USBaseWeapon* CurrentEquippedWeapon;

};
