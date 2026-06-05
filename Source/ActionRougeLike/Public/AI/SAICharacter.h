// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/SAttributeComponent.h"
#include "GameFramework/Character.h"
#include <Actions/SActionComponent.h>
#include "GenericTeamAgentInterface.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/SWorldUserWidget.h"
#include "EFactionTypes.h"
#include "SAICharacter.generated.h"

class UUserWidget;
class USWorldUserWidget;
class USActionComponent;

/// <summary>
/// Seperation between AICharacter and AIController
/// Controller: handles perception, decision logic, team, BB and BT
/// Character: mesh, movement, animation, health, hit reactions?
/// </summary>
UCLASS()
class ACTIONROUGELIKE_API ASAICharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASAICharacter();

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	UPROPERTY(EditAnywhere, Category = "Loot")
	//TSubclassOf<AActor> CoinPickupClass;
	int32 CreditsOnKill;

	UPROPERTY(EditAnywhere, Category = "AI")
	float RotationSpeed = 10.0f;

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	UFUNCTION()
	USBaseInventoryComponent* GetInventoryComp() {return InventoryComponent; };

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPawnSeenFeedback();

	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY(EditAnywhere, Category = "Affiliation")
	EFactionTeam AffiliateTeam = EFactionTeam::EEnemyFaction;
	
protected:
	
	// ---------------------------------
	// -------- Protected Variables -------
	// ---------------------------------

	USWorldUserWidget* EnemySpottedWidget;
	USWorldUserWidget* ActiveHealthBar;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> EnemySpottedWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	USBaseInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParameterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	float DeathReward;

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	/*UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget);*/

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);
};
