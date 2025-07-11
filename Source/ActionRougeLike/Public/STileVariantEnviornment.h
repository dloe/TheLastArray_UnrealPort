// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STileVariantEnviornment.generated.h"

class ASTile;
class USFTileVariantDefinitionData;
class ASTileDoor;

/// <summary>
/// Ideally this will sit on top of the tiles it corresponds to
/// </summary>
UCLASS()
class ACTIONROUGELIKE_API ASTileVariantEnviornment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTileVariantEnviornment();

	UPROPERTY(EditAnywhere, Category = "Tile")
	TArray <ASTile*> VariantEncompassingTiles;

	UPROPERTY(EditAnywhere, Category = "Tile")
	TArray<ASTileDoor*> DoorsToRemove;

	UPROPERTY(EditAnywhere, Category = "Tile")
	USFTileVariantDefinitionData* variantDefinition;

	//this is manually placed in bp to be the starting tile we spawn at (not spawning at the center of this bp)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile")
	FTransform PresetThisSpawnPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
