// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Widgets/SWorldUserWidget.h"
#include "SAIController.generated.h"

//forward declare this
class UBehvaiorTree;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASAIController : public AAIController
{
	GENERATED_BODY()

	public:
	ASAIController();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAIPerceptionComponent* AIPerceptionComp;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EnemySpottedWidgetClass;

	//TODO: Might not need
	virtual FGenericTeamId GetGenericTeamId() const override;
	//need to manually tell the AI how to feel about people. Tell them their feelings, play therapist
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& OtherActor) const override;
	

protected:

	USWorldUserWidget* EnemySpottedWidget;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const;

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleSightSense(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	bool IsValidHostileTarget(AActor* Actor);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeen();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	virtual void BeginPlay() override;
};
