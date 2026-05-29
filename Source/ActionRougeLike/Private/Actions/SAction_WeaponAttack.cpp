// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "Actions/SAction_WeaponAttack.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Weapons/SBaseWeapon.h"
#include "Animation/AnimMontage.h"
#include "Player/SCharacter.h"
#include "AI/SAICharacter.h"
#include "Engine/World.h"
#include "Animations/SAnimInstance.h"

/// <summary>
/// Attack Action Behavior from equipped weapon stored in inventory
/// 
/// A very big key note here is that actions in this system should own the gameplay while the weapons themselves perform 
/// the weapon specific implementation. So player tells action tells weapon to perform attack. 
/// </summary>
/// <param name="Instigator"></param>
void USAction_WeaponAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	USBaseInventoryComponent* InventoryComponent = ResolveInventory(Instigator);

	if (!InventoryComponent)
	{
		//cant perform action if no inventory
		StopAction(Instigator);
		return;
	}

	//if weapon, get weapon stats and run weapon action, else run consumable action
	UInventorySlot* EquippedSlot = InventoryComponent->GetEquippedSlot();

	//can we attack?
	if (!EquippedSlot || !EquippedSlot->IsWeapon)
	{
		//cant do anything else here also
		UE_LOG(LogTemp, Error, TEXT("Failed attacking of slot. Instigator issue... [Class: %s]"), *GetNameSafe(InventoryComponent));
		StopAction(Instigator);
		return;
	}

	//get current weapon
	CurrentEquippedWeapon = Cast<USBaseWeapon>(EquippedSlot->ItemData);
	if (!CurrentEquippedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get weapon. Instigator issue... [Class: %s]"), *GetNameSafe(EquippedSlot->ItemData));
		StopAction(Instigator);
		return;
	}

	StartAttackAfterDelay(Instigator);
}

void USAction_WeaponAttack::OnWeaponAttackFinished(AActor* Instigator)
{
	ASCharacter* Character = Cast<ASCharacter>(Instigator);
	if (Character)
	{
		Character->bIsAiming = true;
	}
	
	StopAction(Instigator);
}

/// <summary>
/// Might have to adjust this later, but the delay to the start of the animation was for certain big attacks with weapons, might need to add the logic for running the big gun anim setup
/// For now basic weapons will have no delay so this isnt in use, AttackDelay_Elasped is called instantly
/// </summary>
/// <param name="InstigatorCharacter"></param>
void USAction_WeaponAttack::AttackDelay_Elasped(AActor* Instigator)
{
	if (!CurrentEquippedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get weapon on attack again. Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
		return;
	}

	CurrentEquippedWeapon->PerformAttack(Instigator, this);

}

/// <summary>
/// Inventory lookup (for player vs AI)
/// </summary>
/// <param name="InstigatingActor"></param>
/// <returns></returns>
USBaseInventoryComponent* USAction_WeaponAttack::ResolveInventory(AActor* InstigatingActor) const
{
	ASCharacter* Character = Cast<ASCharacter>(InstigatingActor);
	if (Character)
	{
			return Character->GetPlayerInventoryComp();
	}

	ASAICharacter* AI = Cast<ASAICharacter>(InstigatingActor);
	if (AI)
	{
		return AI->GetInventoryComp();
	}

	//if we get this far, panic
	UE_LOG(LogTemp, Error, TEXT("WeaponAttackAction: Failed Assignment of InventoryComp. Instigator issue... [Class: %s]"), *GetNameSafe(InstigatingActor));
	return nullptr;
}

/// <summary>
/// Moved delay to be separate
/// </summary>
/// <param name="Instigator"></param>
void USAction_WeaponAttack::StartAttackAfterDelay(AActor* Instigator)
{
	const float AttackDelay = CurrentEquippedWeapon->StartAttacDelay;

	if (AttackDelay <= 0.0f)
	{
		AttackDelay_Elasped(Instigator);
	}
	else {
		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate DelegateAttackDelay;
		DelegateAttackDelay.BindUFunction(this, "AttackDelay_Elasped", Instigator);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, DelegateAttackDelay, CurrentEquippedWeapon->AttacAnimDelay, false);
	}
}
