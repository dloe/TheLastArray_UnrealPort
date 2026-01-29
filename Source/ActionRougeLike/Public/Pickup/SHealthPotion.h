// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup/SPickupBase.h"
#include "Components/StaticMeshComponent.h"
#include "SHealthPotion.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASHealthPotion : public ASPickupBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASHealthPotion();

protected:
	UPROPERTY(EditDefaultsOnly)
	float HealthIncrease;
	UPROPERTY(EditDefaultsOnly)
	float UseCost;


public:	

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;
};
