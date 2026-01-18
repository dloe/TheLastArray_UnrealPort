// Fill out your copyright notice in the Description page of Project Settings.
#include "Interactables/SLevelExit.h"
#include "Gamemodes/SMainGameMode.h"

// Sets default values
ASLevelExit::ASLevelExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ASLevelExit::UnlockObjective()
{
	locked = false;

	//any physical changes we need to make to the exit model or effects or whatever we can do here
}

void ASLevelExit::OnActorLoaded_Implementation()
{

}

//check if gamemode objective is complete then exit
void ASLevelExit::Interact_Implementation(APawn* InstigatorPawn)
{
	
	if (!locked)
	{
		ASMainGameMode* LevelGameMode = Cast<ASMainGameMode>(GetWorld()->GetAuthGameMode());
		//change level, call that function from gamemode
		if (LevelGameMode)
		{
			//check gamemode objective status
			LevelGameMode->ExitLevel();
		}
	}	
}

// Called when the game starts or when spawned
void ASLevelExit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASLevelExit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

