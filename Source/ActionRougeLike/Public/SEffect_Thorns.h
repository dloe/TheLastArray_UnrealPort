// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SEffect_Thorns.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USEffect_Thorns : public USActionEffect
{
	GENERATED_BODY()

	USEffect_Thorns();

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	float ThornReflect;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	

};
