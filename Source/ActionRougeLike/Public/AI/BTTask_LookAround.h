// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LookAround.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UBTTask_LookAround : public UBTTaskNode
{
	GENERATED_BODY()
	
	
public:
	UBTTask_LookAround();

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

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	//local mem
	struct FTaskMemory
	{
		FRotator StartRotation;
		TArray<float> TargetAngles;
		int32 CurrentIndex = 0;
		float RotationSpeed = 60.0f;
	};
};
