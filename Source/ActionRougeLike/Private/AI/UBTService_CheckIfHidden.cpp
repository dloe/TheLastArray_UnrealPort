// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "AI/UBTService_CheckIfHidden.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void UUBTService_CheckIfHidden::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	//check if we are at max health, and update the maxhealth bool in BT
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackBoardComp))
	{
		AActor* TargetActor = Cast <AActor>(BlackBoardComp->GetValueAsObject("TargetActor"));
		AAIController* MyAIController = OwnerComp.GetAIOwner();
		if (ensure(MyAIController) && TargetActor)
		{
			bool bHasLOS = MyAIController->LineOfSightTo(TargetActor);
			BlackBoardComp->SetValueAsBool(IsHiddenKey.SelectedKeyName, bHasLOS);
		}
	}
}
