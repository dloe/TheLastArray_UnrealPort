// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/SPickupBase.h"
#include "Actions/SActionEffect.h"
#include "SActionsPickup.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASActionsPickup : public ASPickupBase
{
	GENERATED_BODY()

	ASActionsPickup();
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Action")
	TSubclassOf<USAction> ActionClass;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
