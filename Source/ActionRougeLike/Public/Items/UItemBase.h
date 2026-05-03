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

UENUM(BlueprintType)
enum class EItemType : uint8 {
	ENone UMETA(DisplayName = "None"),
	EEquipable UMETA(DisplayName = "Equipable"),
	EWeaponRifle UMETA(DisplayName = "WeaponRifle"),
	EWeaponShotgun UMETA(DisplayName = "WeaponShotgun"),
	EWeaponHandheld UMETA(DisplayName = "WeaponHandheld"),
	EWeaponMelee UMETA(DisplayName = "WeaponMelee"),
	EWeaponShlong UMETA(DisplayName = "WeaponShlong"),
	EWeaponFisticuffs UMETA(DisplayName = "WeaponFisticuffs")
};

//Which holser place should they go?
//2 for sidarms
//2 for bigger guns (rifle sized)
UENUM(BlueprintType)
enum class EHoldsterType : uint8 {
	ENone UMETA(DisplayName = "None"),
	ESidearm UMETA(DisplayName = "Small Arms Sidearn"),
	ERifle UMETA(DisplayName = "Heavier Rifle Arms")
};

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

	UFUNCTION(BlueprintCallable, Category = "Item Properties")
	UStaticMeshComponent* GetItemStaticMesh() { return ItemActor->FindComponentByClass<UStaticMeshComponent>(); };

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

	//only populate when item is spawned cause is in hotbar?
	//maybe other instances also
	UPROPERTY(EditAnywhere, Category = "Item Model")
	AActor* ItemActor;

	//A catch all for every time we have a weapon or equiptable item. where does it spawn on the skeletal mesh
	UPROPERTY(EditAnywhere, Category = "Effects")
	FName HandSocketName;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Idle")
	//UAnimMontage* IldeAnimWhenEquipped;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Equip Behavior")
	UAnimSequence* EquipMontage;

	UPROPERTY(EditAnywhere, Category = "Equip Behavior")
	UAnimSequence* DeEquipMontage;
	
	//will need logic to set up which available holster we can keep it in
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EHoldsterType HolsertType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int backSlot = -1;

protected:


	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------


	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
	
};
