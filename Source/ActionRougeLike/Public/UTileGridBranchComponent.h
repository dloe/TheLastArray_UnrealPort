// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UTileGridBranchComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBranchFillGenerated);

class ASTileManager;
class ALocalLevel;
class ASTile;

/// <summary>
/// Sets up grid path addition includes setup for:
/// 
/// - Random rooms and branches
/// - single rooms
/// - secret room (if able)
/// - final setup on doors
/// - de-active inactive rooms
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API UTileGridBranchComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileGridBranchComponent();

	//don't want things to be as statically coupled in manager so split things out with modular classes for each function of the map creation and setup
	UPROPERTY(BlueprintAssignable)

	FOnBranchFillGenerated OnGridAdditionalSetupCompletedEvent; //event to notify when path is created

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void GameMapAdditionalSetup();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void RandomRoomsAndBranchesAdditions();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void AddSingleRooms();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CreateSecretRoom();

	UFUNCTION(BlueprintCallable, Category = "Door Setup")
	void FinalDoorSetupDoors();

	UFUNCTION(BlueprintCallable, Category = "Door Setup")
	void DeactiveInactiveRooms();

	//needs reference to STileManager for accessing Grid and other properties
	UPROPERTY(EditAnywhere)
	ASTileManager* TileManagerRef;

	UPROPERTY(EditAnywhere)
	ALocalLevel* LocalLevelRef;
public:	
	
		
};
