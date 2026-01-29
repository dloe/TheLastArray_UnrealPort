// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UBTService_CheckIfHidden.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UUBTService_CheckIfHidden : public UBTService
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector IsHiddenKey;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
