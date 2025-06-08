// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enviornment/TileGeneration/STileManager.h"
#include "ULevelAssetSetupComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API ULevelAssetSetupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULevelAssetSetupComponent();

protected:
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
	void ActivateEnemies();

	

public:	
	
	UFUNCTION(BlueprintCallable, Category = "Level Asset Population")
	void PopulateGrid();

	UPROPERTY(EditAnywhere)
	ASTileManager* TileManagerRef;
};
