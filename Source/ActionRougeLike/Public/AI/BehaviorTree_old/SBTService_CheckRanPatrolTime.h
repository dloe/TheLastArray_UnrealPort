// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckRanPatrolTime.generated.h"

/**
 * 
 */
UCLASS()
class USBTService_CheckRanPatrolTime : public UBTService
{
	GENERATED_BODY()
	
protected:
	//Time in patrol (mainly for debugging)
	UPROPERTY(EditAnywhere,Category = "AI")
	FBlackboardKeySelector TimeInPassiveRandPatrolKey;

	//How long we stay here
	UPROPERTY(EditAnywhere,Category = "AI")
	FBlackboardKeySelector RandPatrolEndDurationKey;

	//when we stop being in here
	UPROPERTY(EditAnywhere,Category = "AI")
	FBlackboardKeySelector RandPatrolMaxTimeKey;

	//bool dictating which passive path we take
	UPROPERTY(EditAnywhere,Category = "AI")
	FBlackboardKeySelector CanBeInRandPatrolPassiveKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
