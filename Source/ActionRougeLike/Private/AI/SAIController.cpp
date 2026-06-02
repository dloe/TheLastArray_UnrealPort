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


void ASAIController::SetTargetActor(AActor* NewTarget)
{
    GetBlackboardComponent()->SetValueAsObject("TargetActor", NewTarget);
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

}

/// <summary>
/// Specific Sight sense behaviors
/// </summary>
/// <param name="Actor"></param>
/// <param name="Stimulus"></param>
void ASAIController::HandleSightSense(AActor* Actor, FAIStimulus Stimulus)
{
    APawn* SeenPawn = Cast<APawn>(Actor);
    if(!SeenPawn)
        return;
    
    if (Stimulus.WasSuccessfullySensed()) {
        
        if (!IsValidHostileTarget(Actor))
            return;

        // Ignore if target already set
        if (GetTargetActor() != SeenPawn)
        {
            SetTargetActor(SeenPawn);

            //Debug code
            //DrawDebugString(GetWorld(), GetActorLocation(), "PlayerSpotted", nullptr, FColor::White, 4.0f, true);
            //add a draw debug string at the location of the actor so that we have something that shows where the player was spotted

            MulticastPawnSeen();
        }
    }
    else {
        //we lost the player :( big sad
        SetTargetActor(nullptr);

        GetBlackboardComponent()->SetValueAsVector("LastKnownLocation", Stimulus.StimulusLocation);
    }
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
/// Enemy spotted widget UI
/// add a draw debug string at the location of the actor so that we have something that shows where the player was spotted
/// </summary>
void ASAIController::MulticastPawnSeen_Implementation()
{
    EnemySpottedWidget = CreateWidget<USWorldUserWidget>(GetWorld(), EnemySpottedWidgetClass);
    if (EnemySpottedWidget)
    {
        LogOnScreen(this, FString::Printf(TEXT("EnemySpotted Widget")), FColor::Blue);
        EnemySpottedWidget->AttachedActor = this;
        // Index of 10 (or anything higher than default of 0) places this on top of any other widget.
        // May end up behind the minion health bar otherwise.
        EnemySpottedWidget->AddToViewport(10);
    }
}
