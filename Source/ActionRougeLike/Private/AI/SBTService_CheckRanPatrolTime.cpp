// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.

#include "AI/SBTService_CheckRanPatrolTime.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void USBTService_CheckRanPatrolTime::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	bool CountTimeEnabled = OwnerComp.GetBlackboardComponent()->GetValueAsBool(CanBeInRandPatrolPassiveKey.SelectedKeyName);
	

	//if -1 then we never ran OnBecomeRelevant and CountTimeEnabled is automatically active
	if (ensure(BlackBoardComp) && CountTimeEnabled)
	{
		float CurrentTime = OwnerComp.GetWorld()->GetTimeSeconds();
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(TimeInPassiveRandPatrolKey.SelectedKeyName, CurrentTime);
		float EndTime = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(RandPatrolMaxTimeKey.SelectedKeyName);

		//if we exceed the time to remain in rand patrol spots, set bool to swap branches
		if (CurrentTime >= EndTime)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(CanBeInRandPatrolPassiveKey.SelectedKeyName, false);
			//reset to next time
			EndTime = -1;
		}

	}
}

/// <summary>
/// Reset this every time we enter the rand patrol spot branch
/// 
/// Runs once when subtree becomes active
/// This wont run automatically at start if we start in this tree as the active tree
/// </summary>
/// <param name="OwnerComp"></param>
/// <param name="NodeMemory"></param>
void USBTService_CheckRanPatrolTime::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	float CurrentTime = OwnerComp.GetWorld()->GetTimeSeconds();
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(TimeInPassiveRandPatrolKey.SelectedKeyName, CurrentTime);

	//@TODO: could be randomly set? maybe some slight variance might make it nicer
	//@TODO: on become relevant isnt getting hit first :/
	float RandPatrolDurationMax = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(RandPatrolMaxTimeKey.SelectedKeyName);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(RandPatrolMaxTimeKey.SelectedKeyName, CurrentTime + RandPatrolDurationMax);

	//set to true? smells a bit
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(CanBeInRandPatrolPassiveKey.SelectedKeyName, true);
}
