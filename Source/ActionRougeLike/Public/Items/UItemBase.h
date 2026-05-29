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
	EWeapon UMETA(DisplayName = "Weapon"),
	EConsumable UMETA(DisplayName = "Consumable"),
	EQuest UMETA(DisplayName = "Quest"),
	EMisc UMETA(DisplayName = "Misc")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	ENone UMETA(DisplayName = "None"),
	EWeaponRifle UMETA(DisplayName = "WeaponRifle"),
	EWeaponShotgun UMETA(DisplayName = "WeaponShotgun"),
	EWeaponHandheld UMETA(DisplayName = "WeaponHandheld"),
	EWeaponMelee UMETA(DisplayName = "WeaponMelee"),
	EWeaponShlong UMETA(DisplayName = "WeaponShlong"),
	EWeaponFisticuffs UMETA(DisplayName = "WeaponFisticuffs")
};

//Which holster place should they go?
//2 for sidarms
//2 for bigger guns (rifle sized)
UENUM(BlueprintType)
enum class EHoldsterType : uint8 {
	ENone UMETA(DisplayName = "None"),
	ESidearm UMETA(DisplayName = "Small Arms Sidearn"),
	ERifle UMETA(DisplayName = "Heavier Rifle Arms")
};

/**
 * Base Parent item class
 * 
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item Properties")
	bool IsEquippable() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Item Properties")
	EWeaponType GetWeaponType() const;

	virtual EWeaponType GetWeaponType_Implementation() const { return EWeaponType::ENone; };

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = "Equip Behavior")
	UAnimMontage* EquipMont;

	UPROPERTY(EditAnywhere, Category = "Equip Behavior")
	UAnimMontage* DeEquipMont;

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
