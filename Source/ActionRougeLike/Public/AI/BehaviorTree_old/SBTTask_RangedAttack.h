// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	//TODO: this should be better configured to connect to the AICharacter
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxBulletSpread;

	//TODO: this should connect to the AI's weapon on its inventory
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AActor> ProjectileClass;

public:
	USBTTask_RangedAttack();

};
