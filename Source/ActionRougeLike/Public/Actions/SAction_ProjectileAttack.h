// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "SAction_ProjectileAttack.generated.h"

class UAnimMontage;
class UParticleSystem;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
		TSubclassOf<AActor> ProjectileClass;
	UPROPERTY(VisibleAnywhere, Category = "Effects")
		FName HandSocketName;
	UPROPERTY(VisibleAnywhere, Category = "Attack")
		float AttacAnimDelay;
	UPROPERTY(EditAnywhere, Category = "Attack")
		UAnimMontage* AttackAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
		UParticleSystem* CastingEffects;

	UFUNCTION()
	void AttackDelay_Elasped(ACharacter* InstigatorCharacter);

public:
	//UFUNCTION(BlueprintNativeEvent, Category = "Action")
	virtual void StartAction_Implementation(AActor* Instigator) override;

	USAction_ProjectileAttack();


};
