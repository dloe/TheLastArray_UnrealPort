// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
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

	UPROPERTY(EditAnywhere, Category = "Blackbaord Input")
	FBlackboardKeySelector LookAroundSpeedKey;

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

	/*UPROPERTY()
	FRotator StartRotation;
	UPROPERTY()
	TArray<float> TargetAngles;
	UPROPERTY()
	int32 CurrentIndex = 0;*/
	UPROPERTY()
	float RotationSpeed;

	//local mem
	struct FTaskMemory
	{
		FRotator StartRotation;
		TArray<float> TargetAngles;
		int32 CurrentIndex = 0;
	};
};
