// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "AI/BTD_CanShootTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "AI/EFactionTypes.h"

UBTD_CanShootTarget::UBTD_CanShootTarget()
{
	NodeName = "Fire Safety - Can Shoot Target";
}

/// <summary>
/// Shoot raycast out, see if its friendly or not in the raycast
/// </summary>
/// <param name="OwnerComp"></param>
/// <param name="NodeMemory"></param>
/// <returns></returns>
bool UBTD_CanShootTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if(!AICon)
		return false;

	APawn* Pawn = AICon->GetPawn();
	if(!Pawn)
		return false;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject("TargetActor"));
	if(!TargetActor)
		return false;

	//run raycast in helper
	return HasClearShot(Pawn, TargetActor);
}

bool UBTD_CanShootTarget::HasClearShot(AActor* Shooter, AActor* Target) const
{
	FVector Start = Shooter->GetActorLocation();
	FVector End = Target->GetActorLocation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	//don't need to check for shooter if we have the target actor already
	Params.AddIgnoredActor(Shooter); 

	bool bHit = Shooter->GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params
	);

	if (!bHit)
	{
		//we didn't hit ally (or anyone?), can shoot
		return true;
	}

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{
		return true;
	}

	//check if our hit actor is friendly
	if (IsFriendly(Shooter, HitActor))
	{
		//TODO: friendly fire could be something here?
		return false;
	}

	//hit something else like a wall or something
	return true;
}

/// <summary>
/// comparison to see what team everyone is on
/// </summary>
/// <param name="A"></param>
/// <param name="B"></param>
/// <returns></returns>
bool UBTD_CanShootTarget::IsFriendly(AActor* A, AActor* B) const
{
	IGenericTeamAgentInterface* TeamA = Cast<IGenericTeamAgentInterface>(A);
	IGenericTeamAgentInterface* TeamB = Cast<IGenericTeamAgentInterface>(B);

	if (!TeamA || !TeamB)
	{
		//one of our actors isn't on a team, neutral and therefore will return false
		return false;
	}
	if (TeamA->GetGenericTeamId() != TeamB->GetGenericTeamId())
	{
		return false;
	}

	return true;
}
