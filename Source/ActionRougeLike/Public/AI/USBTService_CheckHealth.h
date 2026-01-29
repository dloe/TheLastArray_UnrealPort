// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "USBTService_CheckHealth.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UUSBTService_CheckHealth : public UBTService
{
	GENERATED_BODY()

		//only returns true if we are under this fraction of health
	UPROPERTY(EditAnywhere, Category = "AI")
	float LowHealthFraction;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector MaxHealthKey;

	UUSBTService_CheckHealth();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
};
