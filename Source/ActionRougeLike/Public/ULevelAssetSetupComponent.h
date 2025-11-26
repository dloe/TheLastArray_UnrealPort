// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enviornment/TileGeneration/STileManager.h"
#include "SLocalLevel.h"
#include "Enviornment/PickupAssetData.h"
#include "ULevelAssetSetupComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API ULevelAssetSetupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULevelAssetSetupComponent();

	// ---------------------------------
	// -------- Public Functions -------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void PopulateGridAssets();

	// ---------------------------------
	// ------- Public Variables --------
	// ---------------------------------

	UPROPERTY(EditAnywhere)
	ASTileManager* TileManagerRef;

	UPROPERTY(EditAnywhere)
	ALocalLevel* LocalLevel;

	UPROPERTY(EditAnywhere) //how many items have we placed?
	int PickupsPlaced;

	UPROPERTY(EditAnywhere)
	UPickupAssetData* ItemData;

protected:

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	//idea is that we add by item weight then choose randomly and the weights should be good
	UPROPERTY(EditAnywhere, Category = "Level Asset Population")
	TArray<FItemPickupAsset> LevelItemDropWeightTable;

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void SetupLevelAssetComponent();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void GridAnalysis();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateLevelKey();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateSecretRoom();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateObjectives();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateItems();

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void PlaceItemPickup(UStaticMeshComponent* PickupMarker);

	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void ActivateEnemies();

	UFUNCTION(BlueprintCallable)
	float GetNoiseVec(FVector inputCords);
	
};
