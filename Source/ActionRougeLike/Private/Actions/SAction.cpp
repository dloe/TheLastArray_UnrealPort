// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "Actions/SAction.h"
#include "Actions/SActionComponent.h"
#include <ActionRougeLike/ActionRougeLike.h>
#include <Runtime/Engine/Public/Net/UnrealNetwork.h>

void USAction::Initialize(USActionComponent* NewActionComp)
{
	ActionComp = NewActionComp;

}

/// <summary>
/// Big ideas here, add our tag, run action logic, get timestamp
/// </summary>
/// <param name="Instigator"></param>
void USAction::StartAction_Implementation(AActor* Instigator)
{
	//UE_LOG(LogTemp, Log, TEXT("%s: Running: %s"), *GetNameSafe(Instigator), *GetNameSafe(this));
	//LogOnScreen(this, FString::Printf(TEXT("Started: %s"), *ActionName.ToString()), FColor::Green);

	USActionComponent* Comp = GetOwningComponent();

	//add them all
	//add corresponding tag (is removed from stop action)
	Comp->ActiveGameplayTags.AppendTags(GrantsTags);

	RepData.bIsRunning = true;
	RepData.Instigator = Instigator;
	
	if (GetOwningComponent()->GetOwnerRole() == ROLE_Authority)
	{
		TimeStarted = GetWorld()->TimeSeconds;
	}
	
	//notify BPs of actions
	GetOwningComponent()->OnActionStarted.Broadcast(GetOwningComponent(), this);
}

/// <summary>
/// Action is complete, remove our corresponding tag and run additional stop action logic
/// </summary>
/// <param name="Instigator"></param>
void USAction::StopAction_Implementation(AActor* Instigator)
{
	//check if boolean even on as sanity
	//this will have issues when running on client and server so we will remove it 
	//ensureAlways(bIsRunning);

	USActionComponent* Comp = GetOwningComponent();

	//remove them all
	Comp->ActiveGameplayTags.RemoveTags(GrantsTags);

	RepData.bIsRunning = false;
	RepData.Instigator = Instigator;

	//notify BPs of actions
	GetOwningComponent()->OnActionStopped.Broadcast(GetOwningComponent(), this);
}

/// <summary>
/// Check if any blocked tags are present, cant run aciton when other actions are already running
/// </summary>
/// <param name="Instigator"></param>
/// <returns></returns>
bool USAction::CanStart_Implementation(AActor* Instigator)
{
	//to avoid running this when we have already started action
	if (IsRunning())
	{
		return false;
	}

	USActionComponent* Comp = GetOwningComponent();
	if (Comp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		//if it contains any, return false
		return false;
	}
	else //we are allowed to do this action
		return true;

	
}

/// <summary>
/// This outer is set when creating action via newobject<>
/// </summary>
/// <returns></returns>
UWorld* USAction::GetWorld() const
{
	AActor* Actor = Cast<AActor>(GetOuter());
	if (Actor)
	{
		return Actor->GetWorld();
	}
	return nullptr;
}

/// <summary>
/// Action comp now handles logic, set up when action comp is initialized (external calling when created in BP)
/// </summary>
/// <returns></returns>
USActionComponent* USAction::GetOwningComponent() const
{
	//gotta fix this to work with replication
	//can fix in 2 ways
	//return Cast<USActionComponent>(GetOuter());

	return ActionComp;
}

/// <summary>
/// Multiplayer related
/// </summary>
void USAction::OnRep_RepData()
{
	if (RepData.bIsRunning)
	{
		//TO DO: Fix Instigator
		StartAction(RepData.Instigator);
	}
	else {
		StopAction(RepData.Instigator);
	}

}

/// <summary>
/// using multiplayer replicated data to check
/// </summary>
/// <returns></returns>
bool USAction::IsRunning() const
{
	return RepData.bIsRunning;
}

/// <summary>
/// Which variables are we replicating
/// 
/// </summary>
/// <param name="OutLifetimeProps"></param>
void USAction::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate these to owning comp SAction
	DOREPLIFETIME(USAction, RepData);
	DOREPLIFETIME(USAction, ActionComp);
	DOREPLIFETIME(USAction, TimeStarted);
}