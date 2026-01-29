// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SPickupBase.generated.h"


class USphereComponent;

UCLASS()
class ACTIONROUGELIKE_API ASPickupBase : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ASPickupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	//if we are not done with cooldown yet
	UPROPERTY(ReplicatedUsing = OnRep_Triggered, BlueprintReadonly)
	bool Triggered;

	UFUNCTION()
	void OnRep_Triggered();

	//cooldown delay
	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float TriggerDelay;

	UPROPERTY()
	FTimerHandle TimerHandle;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//implementation from SGameplayInterface
	virtual void Interact_Implementation(APawn* InstigatorPawn);

	virtual void OnTriggerTimer();

	virtual void ShowPowerup();

	void HideAndCooldownPowerup();

	FText GetInteractText_Implementation(APawn* InstigatorPawn);
};
