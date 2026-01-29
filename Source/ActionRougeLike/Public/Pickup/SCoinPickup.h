// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/SPickupBase.h"
#include "SCoinPickup.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASCoinPickup : public ASPickupBase
{
	GENERATED_BODY()
	

private:
	ASCoinPickup();

	

public:
	UPROPERTY(VisibleAnywhere)//editdefaultsonly?
	int PointValue;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

};
