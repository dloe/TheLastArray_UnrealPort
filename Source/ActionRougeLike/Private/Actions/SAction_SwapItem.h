// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "Items/UItemBase.h"
#include "ActorComponents/SBaseInventoryComponent.h"
#include "Animation/AnimSequence.h"
#include "SAction_SwapItem.generated.h"

/**
 * 
 */
UCLASS()
class USAction_SwapItem : public USAction
{
	GENERATED_BODY()
	
public:

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	virtual void StartAction_Implementation(AActor* Instigator) override;

	UFUNCTION()
	void SetInventoryComp(USPlayerInventoryComponent* InventoryCompRef) { InventoryComponent = InventoryCompRef; };

	UFUNCTION(BlueprintImplementableEvent)
	void EquippedItemToItemEvent(EItemType PrevEquippedItemType2, EItemType NewEquippedItemType, UAnimSequence* PrevItemUnequipSeq, UAnimSequence* NewItemEquipMontagSeq);

	UFUNCTION(BlueprintImplementableEvent)
	void EquippedItemToNoneEvent(EItemType PrevEquippedItemType2, UAnimSequence* PrevItemUnequipSeq);

	UFUNCTION(BlueprintImplementableEvent)
	void EquippedNoneToItemEvent(EItemType NewEquippedItemType, UAnimSequence* NewItemEquipMontagSeq);

	//While manipulation of the inventory should only occur at the PlayerInventory comp, the management of the scene and whatnot should be triggered from the action 
	//TODO: move that from the PlayerInventory into here
	UFUNCTION(BlueprintCallable, Category = "Inventory Swap")
	void UNUSED_EquipItemBehavior(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Inventory Swap")
	void OLDDeEquipItemBehavior();


	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	UStaticMeshComponent* SMComp;

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	ASCharacter* Character;

	UPROPERTY(BlueprintReadWrite, Category = "Player Info")
	USPlayerInventoryComponent* InventoryComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Item Swap")
	int HotBarIndexSwappingTo;

	//Helps with transitions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Swap")
	EItemType PrevEquippedItemType = EItemType::ENone;

	UPROPERTY(BlueprintReadWrite, Category = "Item Swap")
	UItemBase* PrevItemRef;

	UPROPERTY(BlueprintReadWrite, Category = "Item Swap")
	int PrevItemIndex;

private:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	UFUNCTION()
	void SwapDelay_Elasped(ACharacter* InstigatorCharacter);

	

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
	UPROPERTY()
	UItemBase* ItemEquipped;
	
};
