// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UItemBase.h"
#include "InventorySlot.generated.h"

class USBaseWeapon;
/**
 * 
 */
 UCLASS(Blueprintable, DefaultToInstanced)
class ACTIONROUGELIKE_API UInventorySlot : public UObject
{

	 GENERATED_BODY()

	 UInventorySlot();

public:

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	UFUNCTION(BlueprintCallable)
	bool isEmptySlot() const { return ItemData == nullptr; };

	UFUNCTION(BlueprintCallable)
	void AssignItem(UItemBase* itemToAssign);

	UFUNCTION(BlueprintCallable)
	USBaseWeapon* GetWeaponData();

	UFUNCTION(BlueprintCallable)
	void Initialize(UItemBase* itemDataP, bool isweaponP, bool isequipableP);

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	
	//item class stores here
	//gets set to null initially
	//all slots need to be present, but they don't all have to have stuff in there
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UItemBase* ItemData;

	//TBD what else
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsWeapon;

	//maybe this one over IsWeapon bool?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsEquipable;

};
