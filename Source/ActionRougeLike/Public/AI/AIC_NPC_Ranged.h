// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "MetaxisAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "../../../../Plugins/Runtime/GameplayStateTree/Source/GameplayStateTreeModule/Public/Components/StateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "AIC_NPC_Ranged.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API AAIC_NPC_Ranged : public AMetaxisAIController
{
	GENERATED_BODY()
	
	public:
	AAIC_NPC_Ranged();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	virtual void OnPossess(APawn* InPawn) override;

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	//state tree AI
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStateTreeAIComponent* StateTreeAI;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAIPerceptionComponent* AIPerceptionComp_Primary;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAIPerceptionComponent* AIPerceptionComp_Proximity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UAIPerceptionComponent* AIPerceptionComp_SecondaryComp;

	//HearingStreakManagerComp - eventually
	//UPROPERTY(VisibleAnywhere, Category = "Components")

	private:
	// ---------------------------------
	// -------- Protected Variables -------
	// ---------------------------------


	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------
	
};
