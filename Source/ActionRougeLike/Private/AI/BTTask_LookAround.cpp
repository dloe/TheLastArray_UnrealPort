// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "AI/BTTask_LookAround.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Chaos/Vector.h"



UBTTask_LookAround::UBTTask_LookAround()
{
	bNotifyTick = true;
	NodeName = "LookAround";

	LookAroundSpeedKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LookAround, LookAroundSpeedKey));
}

uint16 UBTTask_LookAround::GetInstanceMemorySize() const
{
	return sizeof(FTaskMemory);
}

EBTNodeResult::Type UBTTask_LookAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if(!AI)
		return EBTNodeResult::Failed;

	APawn* Pawn = AI->GetPawn();
	if(!Pawn)
		return EBTNodeResult::Failed;

	//access our vars between ticks
	FTaskMemory* Mem = (FTaskMemory*)NodeMemory;

	Mem->StartRotation = Pawn->GetActorRotation();

	//randomize how far we look around. TODO: Configurable?
	float TargetAngle = FMath::FRandRange(50.f, 100.f);

	//figure out the pattern we look in, could add more angles maybe
	Mem->TargetAngles = {-TargetAngle, TargetAngle};
	Mem->CurrentIndex = 0;

	RotationSpeed = AI->GetBlackboardComponent()->GetValueAsFloat(LookAroundSpeedKey.SelectedKeyName);

	return EBTNodeResult::InProgress;
}

void UBTTask_LookAround::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Pawn = AI->GetPawn();
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	FTaskMemory* Mem = (FTaskMemory*) NodeMemory;

	float TargetYaw = Mem->StartRotation.Yaw + Mem->TargetAngles[Mem->CurrentIndex];
	TargetYaw = FRotator::NormalizeAxis(TargetYaw);

	FRotator Current = Pawn->GetActorRotation();
	FRotator Target = FRotator(Current.Pitch, TargetYaw, Current.Roll);

	//smooth it more
	FRotator NewRot = FMath::RInterpConstantTo(Current, Target, DeltaSeconds, RotationSpeed);
	Pawn->SetActorRotation(NewRot);

	//are we close enough to move on?
	float Delta = FMath::FindDeltaAngleDegrees(NewRot.Yaw, TargetYaw);

	if (FMath::Abs(Delta) < 1.0f)
	{
		Mem->CurrentIndex++;

		if (Mem->CurrentIndex >= Mem->TargetAngles.Num())
		{
			//DONE!
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}
}
