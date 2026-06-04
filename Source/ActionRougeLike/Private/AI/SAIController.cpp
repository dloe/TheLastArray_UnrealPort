// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "Delegates/Delegate.h"
#include "Widgets/SWorldUserWidget.h"
#include "../ActionRougeLike.h"
#include "Perception/AISense_Damage.h"
#include "AI/SAICharacter.h"
#include "Perception/AISense_Hearing.h"
#include "TimerManager.h"

ASAIController::ASAIController()
{
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp");
}

void ASAIController::BeginPlay()
{
    Super::BeginPlay();

    if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is Null PTR - Please Assign BehaviorTree in your AI Controller...")))
    {
        RunBehaviorTree(BehaviorTree);
    }


    //APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    //if (MyPawn)
    //{
        //blackboard wrapper
        //need the include for this
    //	GetBlackboardComponent()->SetValueAsVector("MoveToLocation",MyPawn->GetActorLocation());

    //	GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
    //}
}

void ASAIController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ASAIController::OnPerceptionUpdated);
}

void ASAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);

    //if this came from the repositioning flag
    //if (GetBlackboardComponent()->GetValueAsBool("IsRepositioningEngagement"))
    //{
    //    //now we can set the timer
    //    //GetBlackboardComponent()->SetValueAsBool("IsRepositioningEngagement", true);
    //}

    AActor* Target = GetTargetActor();

    //place to throw results
    FActorPerceptionBlueprintInfo Info;
    GetAIPerceptionComponent()->GetActorsPerception(Target, Info);

    //TODO; this is so inefficient
    for (FAIStimulus& Stimuli : Info.LastSensedStimuli)
    {
        if (Stimuli.Type == UAISense_Sight::GetSenseID<UAISense_Sight>() && Stimuli.WasSuccessfullySensed())
        {
            if (!CanSeePlayer() && Target)
            {
                StartForgetTimer(Stimuli);
            }
        }
    }

    
}

FGenericTeamId ASAIController::GetGenericTeamId() const
{
    return FGenericTeamId(2); //enemy team
}

/// <summary>
/// Be the therapist and tell them how to feel about others. Internal monologue
/// 
/// </summary>
/// <param name="OtherActor"></param>
/// <returns></returns>
ETeamAttitude::Type ASAIController::GetTeamAttitudeTowards(const AActor& OtherActor) const
{
    //grab their interface if we can
    const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&OtherActor);
    if (!OtherTeamAgent)
        return ETeamAttitude::Neutral;

    FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();

    // Player team, we hate those guys
    if (OtherTeamID == FGenericTeamId(1))
        return ETeamAttitude::Hostile;

    // Same team, they pretty chill
    if (OtherTeamID == GetGenericTeamId())
        return ETeamAttitude::Friendly;

    //otherwise... eh
    return ETeamAttitude::Neutral;
}

/// <summary>
/// grab our current target
/// </summary>
/// <returns></returns>
AActor* ASAIController::GetTargetActor() const
{
    return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject("TargetActor"));
}

/// <summary>
/// We should know who attacks AI, they investigate if outside attack range
/// </summary>
/// <param name="InstigatorActor"></param>
/// <param name="Delta"></param>
/// <param name="HitLocation"></param>
void ASAIController::ReportDamage(AActor* InstigatorActor, float Delta, FVector HitLocation)
{
    APawn* PawnThatHurtUS = Cast<APawn>(InstigatorActor);
    if(!PawnThatHurtUS)
        return;

    UAISense_Damage::ReportDamageEvent(
        this,
        PawnThatHurtUS,
        InstigatorActor,
        Delta,
        PawnThatHurtUS->GetActorLocation(),
        HitLocation
    );

    if (!IsValidHostileTarget(InstigatorActor))
        return;

    //cancel this timer if we see them
    GetWorld()->GetTimerManager().ClearTimer(ForgetTimerHandle);

    // Ignore if target already set
    if (GetTargetActor() != PawnThatHurtUS)
    {
        //if within line of sight
        if(CanSeePlayer())
            SetTargetActor(PawnThatHurtUS);
        else
            GetBlackboardComponent()->SetValueAsVector("LastKnownLocation", PawnThatHurtUS->GetActorLocation());
        //Debug code
        //DrawDebugString(GetWorld(), GetActorLocation(), "PlayerSpotted", nullptr, FColor::White, 4.0f, true);
        //add a draw debug string at the location of the actor so that we have something that shows where the player was spotted

        //make this call from AICharacter
        ASAICharacter* AIChar = Cast<ASAICharacter>(GetPawn());
        if (AIChar)
        {
            AIChar->MulticastPawnSeenFeedback();
        }
    }
}

void ASAIController::SetTargetActor(AActor* NewTarget)
{
    //cancel this timer if we see them
    GetWorld()->GetTimerManager().ClearTimer(ForgetTimerHandle);

    GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
    GetBlackboardComponent()->SetValueAsVector("LastKnownLocation", NewTarget->GetActorLocation());
}

/// <summary>
/// When the perception is updated
/// </summary>
/// <param name="Actor"></param>
/// <param name="Stimulus"></param>
void ASAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	//only react to sight events
	//this may be scuffed
    //is a switch statement to annoying to implement instead?
	if (Stimulus.Type == UAISense_Sight::GetSenseID<UAISense_Sight>())
	{
		HandleSightSense(Actor, Stimulus);
	}

    /*if (Stimulus.Type == UAISense_Damage::GetSenseID<UAISense_Damage>())
    {
        HandleDamageSense(Actor, Stimulus);
    }*/

    if (Stimulus.Type == UAISense_Hearing::GetSenseID<UAISense_Hearing>())
    {
        HandleHearingSense(Actor, Stimulus);
    }
}

/// <summary>
/// Specific Sight sense behaviors
/// </summary>
/// <param name="Actor"></param>
/// <param name="Stimulus"></param>
void ASAIController::HandleSightSense(AActor* InstigatorActor, FAIStimulus Stimulus)
{
    APawn* SeenPawn = Cast<APawn>(InstigatorActor);
    if(!SeenPawn)
        return;
    
    //returns true if most recent perception update was a positive detection
    //returns false if most recent was lost of detection
    if (Stimulus.WasSuccessfullySensed()) {
        
        if (!IsValidHostileTarget(InstigatorActor))
            return;

        // Ignore if target already set
        if (GetTargetActor() != SeenPawn)
        {
            SetTargetActor(SeenPawn);

            //Debug code
            //DrawDebugString(GetWorld(), GetActorLocation(), "PlayerSpotted", nullptr, FColor::White, 4.0f, true);
            //add a draw debug string at the location of the actor so that we have something that shows where the player was spotted

            //make this call from AICharacter
            ASAICharacter* AIChar = Cast<ASAICharacter>(GetPawn());
            if (AIChar)
            {
                AIChar->MulticastPawnSeenFeedback();
            }
        }
    }
    else {
        AActor* Target = GetTargetActor();
        bool IsRepositioning = GetBlackboardComponent()->GetValueAsBool("IsRepositioningEngagement");
        //a present target action is now lost, check if we were not repositioning
        if (Target != nullptr && Target == SeenPawn &&
        !IsRepositioning)
        {
            //we weren't repositioning, start timer
            StartForgetTimer(Stimulus);
        }
    }
}

/// <summary>
/// if we hear something, investigate it
/// </summary>
/// <param name="Actor"></param>
/// <param name="Stimulus"></param>
void ASAIController::HandleHearingSense(AActor* Actor, FAIStimulus Stimulus)
{
    APawn* HeardPawn = Cast<APawn>(Actor);
    if (!HeardPawn)
        return;

    //if we already have the target, don't need to update?
   /* if (GetTargetActor() == HeardPawn)
	{
		return;
	}*/

     GetBlackboardComponent()->SetValueAsVector("LastKnownLocation", Stimulus.StimulusLocation);
}

/// <summary>
/// TODO: Expand out who the AI can target
/// Could have AI attack each other
/// </summary>
/// <param name="Actor"></param>
/// <returns></returns>
bool ASAIController::IsValidHostileTarget(AActor* Actor)
{
    APawn* PotentialPawn = Cast<APawn>(Actor);
    if (!PotentialPawn)
        return false;

    if(GetTargetActor() == PotentialPawn)
        return false;

    //team interface?
    //are they on our team?
    IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Actor);
    if (!TeamAgent)
        return false;

    ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(*Actor);
    if (Attitude == ETeamAttitude::Hostile)
        return true;
    else {
        return false;
    }
}

/// <summary>
/// check if player is in los
/// </summary>
/// <returns></returns>
bool ASAIController::CanSeePlayer()
{
    AActor* Target = GetTargetActor();
    if (!Target)
    {
        return false;
    }

    //place to throw results
    FActorPerceptionBlueprintInfo Info;
    GetAIPerceptionComponent()->GetActorsPerception(Target, Info);

    //did we see em?
    for (FAIStimulus& Stimuli : Info.LastSensedStimuli)
    {
        if (Stimuli.Type == UAISense_Sight::GetSenseID<UAISense_Sight>() && Stimuli.WasSuccessfullySensed())
        {
            return true;
        }
    }
    return false;
}   

/// <summary>
/// When we loose sight of player and not moving to new engagement loc, start this timer
/// </summary>
/// <returns></returns>
void ASAIController::StartForgetTimer(FAIStimulus Stimulus)
{
    //make delegate to pass in param for ForgetTargetActor
    FTimerDelegate ForgetDelegate;
    ForgetDelegate.BindUFunction(this, "ForgetTargetActor", Stimulus);

    GetWorld()->GetTimerManager().SetTimer(
        ForgetTimerHandle,
        ForgetDelegate,
        ForgetSightTargetTime,
        false
    );
}

/// <summary>
/// if timer goes off, that means we actualy lost sight of player
/// </summary>
/// <param name="Stimulus"></param>
void ASAIController::ForgetTargetActor(FAIStimulus Stimulus)
{
    //we lost the player :( big sad
    SetTargetActor(nullptr);

    //can investigate players last location
    GetBlackboardComponent()->SetValueAsBool("InvestigatedForPlayer", false);
    GetBlackboardComponent()->SetValueAsVector("LastKnownLocation", Stimulus.StimulusLocation);

}

