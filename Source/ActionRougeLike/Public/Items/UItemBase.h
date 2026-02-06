// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//#include "Engine/DataAsset.h"
#include "UItemBase.generated.h"

/**
 * Should this be a UObject or AActor?
 * 
 * if its actor it can be the parent for ASBaseWeapon?
 */
UCLASS(Blueprintable, BlueprintType, DefaultToInstanced)
class ACTIONROUGELIKE_API UItemBase : public UObject
{
	GENERATED_BODY()
	
	public:
	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	UPROPERTY(EditAnywhere, Category = "Item Properties")
	FName ItemName;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FName ItemDescription;

	//icon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSoftObjectPtr<UTexture2D> Icon;

		//storing blueprint of physical weapon model and mesh
	//cant i just get the static mesh comp from this guy?
	UPROPERTY(EditAnywhere, Category = "Item Model")
	TSubclassOf<AActor> ItemActorSubclass;

	UPROPERTY(EditAnywhere, Category = "Item Model")
	AActor* ItemActor;

	//A catch all for every time we have a weapon or equiptable item. where does it spawn on the skeletal mesh
	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName = "Muzzle_01";

	UPROPERTY(EditAnywhere, Category = "Idle")
	UAnimMontage* IldeAnimWhenEquipped;

protected:


	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------


	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
	UPROPERTY(EditAnywhere, Category = "Item Model")
	UStaticMeshComponent* ItemStaticMesh;
};
