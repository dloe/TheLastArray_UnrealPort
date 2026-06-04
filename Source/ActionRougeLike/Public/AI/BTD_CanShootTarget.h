// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_CanShootTarget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UBTD_CanShootTarget : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTD_CanShootTarget();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

protected:

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	bool HasClearShot(AActor* Shooter, AActor* Target) const;
	bool IsFriendly(AActor* A, AActor* B) const;

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------
};
