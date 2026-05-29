// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Actions/SActionComponent.h"
#include "SAnimInstance.generated.h"

class ASCharacter;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	
protected:

	// ---------------------------------
	// -- Internal state / variables --
	// ---------------------------------

	//check action component if we are stunned
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsStunned;

	//check action component if we are reloading
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsReloading;

	//check if character has isaiming on (TODO: might want to find a better place to put this inside player comp or something)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool bIsAiming;

	//note: is updated when we finish the firing animation to be false (TODO: Could be inefficient for SUPER fast firing weapons, maybe is set to false different?)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bWantsToFire;

	//reading in player input via player vp. used in loco state machine in animBP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bWantsToMove;


	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	USActionComponent* ActionComp;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ASCharacter* Character;

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;

};
