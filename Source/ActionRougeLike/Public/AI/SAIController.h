// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAIController.generated.h"

//forward declare this
class UBehvaiorTree;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASAIController : public AAIController
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	virtual void BeginPlay() override;
};
