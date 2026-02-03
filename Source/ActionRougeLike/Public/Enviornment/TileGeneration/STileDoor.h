// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASTileWall.h"
#include "STileDoor.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASTileDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTileDoor();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Door Info")
	bool DoorActive = false;


	UPROPERTY(EditAnywhere, Category = "Door Info")
	ASTileWall* DoorsConnector;

	UPROPERTY(EditAnywhere, Category = "Door Info")
	bool DestroyConnectorWalls = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
