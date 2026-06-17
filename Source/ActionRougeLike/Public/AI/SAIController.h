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
#include "SAICharacter.h"
#include "EFactionTypes.h"
#include "SAIController.generated.h"

//forward declare this
class UBehvaiorTree;
class ASAICharacter;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASAIController : public AAIController
{
	GENERATED_BODY()

	public:
	ASAIController();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Perception")
	void ReportDamage(AActor* InstigatorActor, float Delta, FVector HitLocation);

	//TODO: Might not need
	virtual FGenericTeamId GetGenericTeamId() const override;
	//need to manually tell the AI how to feel about people. Tell them their feelings, play therapist
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& OtherActor) const override;

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UAIPerceptionComponent* AIPerceptionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Stimuli Related")
	float ForgetSightTargetTime;

	UPROPERTY(EditAnywhere, Category = "Affiliation")
	EFactionTeam AffiliateTeamAIC = EFactionTeam::EEnemyFaction;

	UPROPERTY(EditAnywhere, Category = "Functional Testing")
	bool TestDisableBrain = false;

protected:

	// ---------------------------------
	// -------- Protected Variables -------
	// ---------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	FTimerHandle ForgetTimerHandle;
	FTimerHandle DelayEngagementTimerHandle;

	/*UPROPERTY()
	APawn* SeenPawn;*/

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetTargetActor() const;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleSightSense(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleHearingSense(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	bool IsValidHostileTarget(AActor* Actor);

	UFUNCTION()
	bool CanSeePlayer();

	UFUNCTION()
	void StartForgetTimer(FAIStimulus Stimulus);

	UFUNCTION()
	void ForgetTargetActor(FAIStimulus Stimulus);
};
