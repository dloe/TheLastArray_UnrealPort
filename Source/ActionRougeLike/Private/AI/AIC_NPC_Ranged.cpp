// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "AI/AIC_NPC_Ranged.h"

AAIC_NPC_Ranged::AAIC_NPC_Ranged()
{
	StateTreeAI = CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAI");
	AIPerceptionComp_Primary = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp_Primary");
	AIPerceptionComp_Proximity = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp_Proximity");
	AIPerceptionComp_SecondaryComp = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp_SecondaryComp");
}

void AAIC_NPC_Ranged::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	//assign values
	

}

