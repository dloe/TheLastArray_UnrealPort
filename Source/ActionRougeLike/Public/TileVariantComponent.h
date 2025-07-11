// Fill out your copyright notice in the Description page of Project Settings.

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
};
