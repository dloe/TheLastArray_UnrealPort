// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UTilePathSetupComp.generated.h"

class ASTileManager;
class ASTile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPathGenerated);

/// <summary>
/// Handles Main path includes setup for
/// 
/// - Choosing start and end rooms
/// - generate main path
/// - create spawn room
/// - 
/// </summary>
UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API UTilePathSetupComp : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UTilePathSetupComp();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void TilePathGeneration();

	//don't want things to be as statically coupled in manager so split things out with modular classes for each function of the map creation and setup
	UPROPERTY(BlueprintAssignable)

	FOnPathGenerated OnPathGeneratedEvent; //event to notify when path is created


	//For debug if we want to hard code a specific side to test, we set to 0 -3 otherwise it will get overridden
	UPROPERTY(EditAnywhere, Category = "Tile Path Generation")
	int StartRoomSide = -1;

protected:

	//needs reference to STileManager for accessing Grid and other properties
	UPROPERTY(EditAnywhere)
	ASTileManager* TileManagerRef;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	int FailsafeCount = 0;

	//list of possible starting tiles - DO DO: PROTECT THIS LATER
	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	TArray<ASTile*> BackTrackHistory;

	UPROPERTY(EditAnywhere, Category = "Tile Generation")
	FTransform doorTransform;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void ChooseStartEndRooms();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void GeneratePath();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CreateSpawnRoom();

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void CheckTile(ASTile* TileToAdd, TArray<ASTile*>& CurrentPath);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	bool AddTileToPath(ASTile* TileToAdd);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	TArray <int> Reshuffle(TArray <int> ar);

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	TArray <ASTile*> ReshuffleTiles(TArray <ASTile*> ar);

public:	
	

};
