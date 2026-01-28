// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "Gamemodes/SMainGameMode.h"
#include "Player/SPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "AI/SAICharacter.h"
#include "Attributes/SAttributeComponent.h"
#include "SGameModeBase.h"
#include <ActionRougeLike/Public/AI/SAICharacter.h>
#include "EngineUtils.h"
#include "SSaveGame.h"
#include "GameFramework/GameState.h"
#include <ActionRougeLike/Public/Player/SCharacter.h>
#include <ActionRougeLike/Public/SGameplayInterface.h>
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>
#include "ScriptableObjects/SMonsterData.h"
#include <ActionRougeLike/ActionRougeLike.h>
#include <Runtime/Engine/Classes/Engine/AssetManager.h>

void ASMainGameMode::CheckObjective()
{
	UE_LOG(LogTemp, Log, TEXT("Checking objective completion stats..."));
	if (LevelObjective->CheckForObjectiveCompletion())
	{
		//returns true, objective complete can now extract
		UE_LOG(LogTemp, Log, TEXT("OBJECTIVE COMPLETED. PLAYER WILL BE ABLE TO NOW EXTRACT"));

		//can now interact and exit level
		LevelExitRef->UnlockObjective();
	}
}

void ASMainGameMode::ExitLevel()
{
	//graphic animation maybe?

	UE_LOG(LogTemp, Log, TEXT("EXITTED LEVEL!"));

	//load level

	//save out data? level streaming may indicate we use a different system, tbd

	//use level streaming, NEED MORE RESEARCH ON
	UWorld* World = GetWorld();
	if (World)
	{
		//run swap blueprint function

		//loading screen 
		//create widget
		//add it to view
		TransitionToTrainLevel(); //implemented in bp

	}
}

/// <summary>
/// update stats, remove enemy from current list
/// </summary>
/// <param name="InstigatorActor"></param>
/// <param name="MinionCost"></param>
void ASMainGameMode::KillNormalEnemyEvent(AActor* InstigatorActor, int MinionCost, AActor* EnemyKilled)
{
	AController* InstigatingController = InstigatorActor->GetInstigatorController();
	if (ensure(InstigatingController) && InstigatingController->IsPlayerController()) {
		APlayerController* PC = Cast<APlayerController>(InstigatingController);
		if(ensure(PC)) {
			ASPlayerState* PS = Cast<ASPlayerState>(PC->PlayerState);
			PS->AddCredits(MinionCost);
		} else {
			UE_LOG(LogTemp, Error, TEXT("KillNormalEnemyEvent: issue with player controller cast"));
		}
	}
	else
	{
		//transfer minion cost to the guy that killed our other enemy (if possible)
		ASAICharacter* InstigatingEnemy = Cast<ASAICharacter>(EnemyKilled);
		if (InstigatingEnemy)
		{
			InstigatingEnemy->CreditsOnKill += MinionCost;
		}

		UE_LOG(LogTemp, Log, TEXT("%s had friendly fire incident from %s"), *GetNameSafe(EnemyKilled), *GetNameSafe(InstigatorActor));
	}

	//remove from enemies active list
	LevelSetupComp->SpawnedEnemiesInLevel.Remove(EnemyKilled);

	CheckObjective();
}

//cheat, runnable via cmd
void ASMainGameMode::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); //@fixme pass in player? for kill credit
		}
	}
}

//skeletal structure for later from old gamemode ref
void ASMainGameMode::WriteSaveGame()
{
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break; // single player only at this point
		}
	}


	CurrentSaveGame->SavedActors.Empty();

	//iterate the entire world of actors
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// only interested in our 'gameplay actors'
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetActorTransform();

		//pass array to fill with data from actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		//marks it as only looking for those saving props UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		//converts actors SaveGame UPROPERTIES into binrary array
		Actor->Serialize(Ar);


		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASMainGameMode::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame data"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame data"));

		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			// only interested in our 'gameplay actors'
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}

			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					Actor->SetActorTransform(ActorData.Transform);

					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					//marks it as only looking for those saving props UPROPERTY(SaveGame)
					Ar.ArIsSaveGame = true;
					//converts binary array abck into actors SaveGame UPROPERTIES 
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					break;
				}
			}

		}
	}
	else
	{
		CurrentSaveGame = Cast < USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created new SaveGame data"));
	}



}
