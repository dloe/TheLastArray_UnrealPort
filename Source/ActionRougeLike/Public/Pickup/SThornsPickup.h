// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Pickup/SPickupBase.h"
#include "Actions/SActionEffect.h"
#include "SThornsPickup.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASThornsPickup : public ASPickupBase
{
	GENERATED_BODY()
	
private:
	ASThornsPickup();

public:
	UPROPERTY(VisibleAnywhere)
	int ThornDuration;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> ThornActionClass;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
