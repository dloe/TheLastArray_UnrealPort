// Fill out your copyright notice in the Description page of Project Settings.


#include "TileVariantComponent.h"

// Sets default values for this component's properties
UTileVariantComponent::UTileVariantComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	UE_LOG(LogTemp, Log, TEXT("test b"));
	// ...
	
	UE_LOG(LogTemp, Log, TEXT("test a"));
}


// Called when the game starts
void UTileVariantComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}


// Called every frame
void UTileVariantComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTileVariantComponent::SetVariables()
{
	TileVariantTiersLocal = {
			FTileVariantDefinitionRow(Stage1_Highest, 0, 1),
			FTileVariantDefinitionRow(Stage2, 0, 1),
			FTileVariantDefinitionRow(Stage3, 1,2),
			FTileVariantDefinitionRow(Stage4,1,2),
			FTileVariantDefinitionRow(Stage5_Lowest, 25,25)
	};
}

