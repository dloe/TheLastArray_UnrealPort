// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "GameFramework/Actor.h"
#include "SLevelExit.generated.h"

UCLASS()
class ASLevelExit : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	// Sets default values for this actor's properties
	ASLevelExit();

	void Interact_Implementation(APawn* InstigatorPawn);

	void OnActorLoaded_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Objective")
	void UnlockObjective();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

protected:
	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool locked = false;


};
