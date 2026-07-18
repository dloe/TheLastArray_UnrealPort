// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "AI/SBTTask_SetPatrolPath.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/SAICharacter.h"

/// <summary>
/// Will generate and find the patrol points for our AI to move around in the set patrol passive state
/// </summary>
/// <param name="OwnerComp"></param>
/// <param name="NodeMemory"></param>
/// <returns></returns>
EBTNodeResult::Type USBTTask_SetPatrolPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//run the three queries

	//lets try something and cache the ownercomp here
	CachedOwnerComp = &OwnerComp;
	//prep collected points
	CollectedPoints.Reset();

	//APawn* Pawn = OnwerComp//Cast<APawn>(OwnerComp.GetAIOwner());
	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Error, TEXT("SetPatrolPath failure, cannot setup..."));


		return EBTNodeResult::Failed;
	}

	//get patrol route interface
	ASAICharacter* AIChar = Cast<ASAICharacter>(Pawn);
	AActor* PatrolRoute = AIChar->PatrolPath_BP;
	SplineRouteComp = PatrolRoute->GetComponentByClass<USplineComponent>();
	if (!SplineRouteComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Canot access spline comp from patrol BP in AI..."));
		return EBTNodeResult::Failed;
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolLocContextKey.SelectedKeyName, Pawn->GetActorLocation());
	PointsOnSpline = SplineRouteComp->GetNumberOfSplinePoints();

	RunQueryAtContext();

	// if everything failed to start
	if (PendingQueries <= 0)
		return EBTNodeResult::Failed;

	return EBTNodeResult::InProgress;
}

/// <summary>
/// collect query results
/// </summary>
/// <param name="QueryInstance"></param>
/// <param name="Status"></param>
void USBTTask_SetPatrolPath::HandleQueryFinished(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type Status)
{
	if (Status == EEnvQueryStatus::Success)
	{
		TArray<FVector> Locations;
		Locations = QueryInstance->GetResultsAsLocations();

		if (Locations.Num() > 0)
		{
			FVector NewPoint = Locations[0];
			CollectedPoints.Add(NewPoint);

			//update context (so next point in spline) to search around
			CachedOwnerComp->GetBlackboardComponent()->SetValueAsVector(PatrolLocContextKey.SelectedKeyName, NewPoint);
		}

		if (SplineSpotRouteIndex > PointsOnSpline)
		{
			BuildFinalPatrolSpline();
		}
		else {
			RunQueryAtContext();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Patrol Spot Gen Query failed, status: %s"), *UEnum::GetValueAsString(Status));
	}
}

/// <summary>
/// Once all queries are done running we build out the final spline
/// </summary>
void USBTTask_SetPatrolPath::BuildFinalPatrolSpline()
{
	SplineRouteComp->ClearSplinePoints();

	for (const FVector& Loc : CollectedPoints)
	{
		SplineRouteComp->AddSplinePoint(Loc, ESplineCoordinateSpace::World);
	}

	SplineRouteComp->UpdateSpline();
}

/// <summary>
/// Run query with context location into account
/// </summary>
/// <param name="Context"></param>
void USBTTask_SetPatrolPath::RunQueryAtContext()
{
	AAIController* AICon = CachedOwnerComp->GetAIOwner();
	APawn* Pawn = AICon ? AICon->GetPawn() : nullptr;
	if (!Pawn)
	{
		FinishLatentTask(*CachedOwnerComp , EBTNodeResult::Failed);
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* Wrapper =
		UEnvQueryManager::RunEQSQuery(
			Pawn,
			QueryToFindPatrolSpot,
			Pawn,
			EEnvQueryRunMode::SingleResult,
			nullptr);

	if (!Wrapper)
	{
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Failed);
		return;
	}

	// Pass context via named params
	//Wrapper->SetNamedParam(TEXT("SearchOriginX"), Context.X);
	//Wrapper->SetNamedParam(TEXT("SearchOriginY"), Context.Y);
	//Wrapper->SetNamedParam(TEXT("SearchOriginZ"), Context.Z);

	Wrapper->GetOnQueryFinishedEvent().AddDynamic(
		this,
		&USBTTask_SetPatrolPath::HandleQueryFinished
	);
}
