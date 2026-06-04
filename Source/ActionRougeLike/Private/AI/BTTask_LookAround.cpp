// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "AI/BTTask_LookAround.h"
#include "AIController.h"



UBTTask_LookAround::UBTTask_LookAround()
{
	bNotifyTick = true;
	NodeName = "LookAround";
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

	FTaskMemory* Mem = (FTaskMemory*)NodeMemory;

	Mem->StartRotation = Pawn->GetActorRotation();

	//figure out teh pattern we look in
	Mem->TargetAngles = {-45.f, 45.f, 0.f};
	Mem->CurrentIndex = 0;

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
	FRotator Current = Pawn->GetActorRotation();
	FRotator Target = FRotator(Current.Pitch, TargetYaw, Current.Roll);

	//smooth it more
	FRotator NewRot = FMath::RInterpConstantTo(Current, Target, DeltaSeconds, Mem->RotationSpeed);
	Pawn->SetActorRotation(NewRot);

	//are we close enough to move on?
	if (FMath::Abs(NewRot.Yaw - TargetYaw) < 1.0f)
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
