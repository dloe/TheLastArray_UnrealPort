// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "Components/SplineComponent.h"
#include "SBTTask_SetPatrolPath.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USBTTask_SetPatrolPath : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	//UPROPERTY(EditAnywhere, Category = "EQS")
	//TArray<UEnvQuery*> QueryTempalates;

	UPROPERTY(EditAnywhere, Category = "EQS")
	UEnvQuery* QueryToFindPatrolSpot;

	UPROPERTY(EditAnywhere,Category = "AI")
	FBlackboardKeySelector PatrolLocContextKey;

protected:

	//query function
	UFUNCTION()
	void HandleQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type Status);

	UFUNCTION()
	void BuildFinalPatrolSpline();

	UFUNCTION()
	void RunQueryAtContext();

	UPROPERTY()
	UBehaviorTreeComponent* CachedOwnerComp;

	//UPROPERTY()
	//FVector CurrentContext;
	
	UPROPERTY()
	int PendingQueries;

	//@TODO: might not need some of these vars
	UPROPERTY()
	USplineComponent* SplineRouteComp;

	UPROPERTY()
	int PointsOnSpline;

	UPROPERTY()
	int SplineSpotRouteIndex;

	UPROPERTY()
	TArray<FVector> CollectedPoints;
};
