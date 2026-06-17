// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFTileVariantDefinitionData.h"
#include "TileVariantComponent.generated.h"


UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROUGELIKE_API UTileVariantComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileVariantComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	// ---------------------------------
	// ------- Public Variables --------
	// ---------------------------------

	//tiers of different variants in 2d array
	UPROPERTY(EditAnywhere, Category = "Tile Variant Data")
	TArray<FTileVariantDefinitionRow> TileVariantTiersLocal;

	//tiers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Variant Data")
	TArray <USFTileVariantDefinitionData*> Stage1_Highest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Variant Data")
	TArray <USFTileVariantDefinitionData*> Stage2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Variant Data")
	TArray <USFTileVariantDefinitionData*> Stage3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Variant Data")
	TArray <USFTileVariantDefinitionData*> Stage4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Variant Data")
	TArray <USFTileVariantDefinitionData*> Stage5_Lowest;

	// ---------------------------------
	// -------- Public Functions -------
	// ---------------------------------

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "ArrayCreation")
	void SetTierDataVariables();
};
