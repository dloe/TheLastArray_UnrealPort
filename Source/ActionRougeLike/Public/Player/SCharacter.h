// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/SAttributeComponent.h"
#include "Interactables/SInteractionActorComponent.h"
#include "GameFramework/Character.h"
#include <Actions/SActionComponent.h>
#include "Particles/ParticleSystemComponent.h"
#include "SPlayerState.h"
#include "Actions/SActionEffect.h"
#include "ActorComponents/SPlayerInventoryComponent.h"
#include "GenericTeamAgentInterface.h"
#include "SCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;
class USActionComponent;

UCLASS()
class ACTIONROUGELIKE_API ASCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
protected:

	// ---------------------------------
	// -------- Protected Variables -------
	// ---------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectSpellCastComp;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	//interaction
	UPROPERTY(VisibleAnywhere)
	USInteractionActorComponent* InteractiveComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Components")
	USActionComponent* ActionComp;

	//this is where our weapon inventory should reside
	//or general inventory? or access to that inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	USPlayerInventoryComponent* PlayerInventoryComponent;

	//TO DO
	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "Player")
	ASPlayerState* PState;

	UPROPERTY(EditDefaultsOnly)
	int HitFlashSpeed;

	UPROPERTY(EditDefaultsOnly)
	int BlackHoleRageCost;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName TimeToHitParameterName;

	UPROPERTY(EditDefaultsOnly)
	FColor HitFlashColor;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> ThornActionClass;

	// ---------------------------------
	// -------- Helper Functions -------
	// ---------------------------------

	//actions
	void SprintStart();
	void SprintStop();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);

	void BlackholeAttack();

	void TeleportAbility();

	void PrimaryAttack();

	void PrimaryInteract();

	void PrimaryReload();

	void SelectHotbar1() { SetEquippedHotbar(0); };
	void SelectHotbar2() { SetEquippedHotbar(1); };
	void SelectHotbar3() { SetEquippedHotbar(2); };
	void SelectHotbar4() { SetEquippedHotbar(3); };
	void SetEquippedHotbar(int input);

	UFUNCTION()
	void SetLeftHandIKTrans();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION()
	void HandleDeath();

	//slightly better way to bind your events in timeline
	virtual void PostInitializeComponents() override;

	virtual FVector GetPawnViewLocation() const override;

public:	

	// Sets default values for this character's properties
	ASCharacter();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------
	virtual FGenericTeamId GetGenericTeamId() const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

	UFUNCTION()
	USPlayerInventoryComponent* GetPlayerInventoryComp() {return PlayerInventoryComponent; };

	UFUNCTION()
	USActionComponent* GetActionComp() { return ActionComp; };

	UFUNCTION(BlueprintImplementableEvent)
	void PostInitBlueprint();

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------
	/// <summary>
	/// When the player shoots the gun, we want a post fire blend animation to run where the player ironsights idle
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack Animation")
	bool bIsAiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bIsPlayingEmote;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Left Hand Adjustments")
	FTransform WeaponLeftHandIKTrans;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bPrintCurrentAnimGraphLayer = false;
	

};
