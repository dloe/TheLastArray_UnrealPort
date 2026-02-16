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

/// <summary>
/// Attack Action Behavior from equipped weapon stored in inventory
/// 
/// 
/// </summary>
/// <param name="Instigator"></param>
void USAction_WeaponAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	//if instigator is type player
	ASCharacter* Character = Cast<ASCharacter>(Instigator);
	ASAICharacter* AI = Cast<ASAICharacter>(Instigator);
	if (Character)
	{
		InventoryComponent = Character->GetPlayerInventoryComp();
	}
	else if(AI) { //AI Inventory
		InventoryComponent = AI->GetInventoryComp();
	}
	else {
		//throw error?
		UE_LOG(LogTemp, Error, TEXT("Failed Assignment of InventoryComp. Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
	}

	//if weapon, get weapon stats and run weapon action, else run consumable action
	UInventorySlot* ItemEquipped = InventoryComponent->GetEquippedItem();
	if(ItemEquipped->IsWeapon) {

		EquipedWeaponFromInventory = Cast<USBaseWeapon>(ItemEquipped->ItemData);

		ensure(EquipedWeaponFromInventory);
		EquipedWeaponAttackAnimAction = EquipedWeaponFromInventory->AttackAnim;
		EquipedWeaponCastingEffectsAction = EquipedWeaponFromInventory->CastingEffects;
		EquipedSpawnSocketNameAction = EquipedWeaponFromInventory->WeaponMuzzleSocketName;
		EquipedAttacAnimDelayAction = EquipedWeaponFromInventory->AttacAnimDelay;
		EquipedWeaponProjectileSubclassAction = EquipedWeaponFromInventory->WeaponProjectile;
		EquipedWeaponCasing = EquipedWeaponFromInventory->EjectedCasingActor;
		EquipedWeaponStaticMesh = EquipedWeaponFromInventory->GetItemStaticMesh();


		//get our attacking character
		//todo: put in character condition (or combine with ai one?)
		if (Character)
		{
			Character->PlayAnimMontage(EquipedWeaponAttackAnimAction); //start animation, then wait until animation is finished to spawn projectile physically

			if (Character->HasAuthority()) {
				FTimerHandle TimerHandle_AttackDelay;
				FTimerDelegate Delegate;
				
				//when timer finishes, spawn projectile
				if(EquipedAttacAnimDelayAction != 0.0f) {
					//unless there is a 'warm up' animation that has to run before we can fire the weapon or make the attack, this will most likely be near 0
					Delegate.BindUFunction(this, "AttackDelay_Elasped", Character);
					GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, EquipedAttacAnimDelayAction, false);
				} else {
					//run immediately if no delay is there
					AttackDelay_Elasped(Character);
				}
			}
		}
		else if (AI)
		{
			//panick
		}
	}
	else {
		//if consumable
		//this behavior TBD
		//expect this to move around as i find a better way to incorporate consumables (drugs, power ups, train support charges, etc)
	}
}

/// <summary>
/// After animation players we spawn projectiles or do the attack physically 
/// </summary>
/// <param name="InstigatorCharacter"></param>
void USAction_WeaponAttack::AttackDelay_Elasped(ACharacter* InstigatorCharacter)
{
	//very similar behavior as the magic projectile, except we get alot of the properties from teh weapon instead of beiung
	//stored in this action class

	if (ensureAlways(EquipedWeaponProjectileSubclassAction))
	{
		//use weapons projectiles name instead of 'muzzle'
		const FVector HandLocation = EquipedWeaponStaticMesh->GetSocketLocation(EquipedSpawnSocketNameAction);

		//EPSCPoolMethod PoolingMethod; //defaults to none
		//attach location can be KeepWorldPosition or KeepRelativeOffset
		// scale shouldnt be needed if we use keepworldposition

		//have particle effect in players hand when shooting projectile
		//should be where our hand socket is, where the projectile spawns in
		// cast root component as a scene component?
		//UParticleSystem* EmitterTemplate, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoDestroy, EPSCPoolMethod PoolingMethod, bool bAutoActivateSystem
		// instructor used CastSpellVFX, GetMesh,HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true, EPSCPoolMethod::None, true);
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		//spawn particle effect from hand socket on mesh

		UGameplayStatics::SpawnEmitterAttached(EquipedWeaponCastingEffectsAction, EquipedWeaponStaticMesh, EquipedSpawnSocketNameAction, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);


		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;


		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);


		//for trace
		float basicTraceRange = 5000;

		//the crosshair is at the forward vector of camera so might as well just use that
		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();//CameraComp->GetComponentLocation();

		//endpoint far into the look at distance (not to far, still somewhat towards crosshair on a miss)
		FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * basicTraceRange);

		FHitResult hitCam;
		//if we dont hit anything just use the end of the trace line
		FVector projectileEndLocale = TraceEnd;
		//if(GetWorld()->LineTraceSingleByObjectType(hitCam, TraceStart, TraceEnd, ObjectQueryParams))
		//line up hand with where we are aiming for accuracy on spawning of our magic fireball
		if (GetWorld()->SweepSingleByObjectType(hitCam, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params))
		{
			//if we got a hit, then we now have a start (handlocation), and end point (hitCam.ImpactPoint)
			projectileEndLocale = hitCam.ImpactPoint;
		}

		//OLD - rotation is looking at that point we now have
		//FRotator ProjectileSpawnRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, projectileEndLocale);

		//more accurate version
		FRotator ProjRotation = FRotationMatrix::MakeFromX(projectileEndLocale - HandLocation).Rotator();


		//replaced GetControlRotation with our new target rotation
		const FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		AActor* T = GetWorld()->SpawnActor<AActor>(EquipedWeaponProjectileSubclassAction, SpawnTM, SpawnParams);

		EjectCasing(InstigatorCharacter);
	}

	StopAction(InstigatorCharacter);
}

void USAction_WeaponAttack::EjectCasing(ACharacter* InstigatorCharacter)
{
	const FVector CasingLocation = EquipedWeaponStaticMesh->GetSocketLocation("");
	const FTransform SpawnTM = FTransform(CasingLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = InstigatorCharacter;

	AActor* casing = GetWorld()->SpawnActor<AActor>(EquipedWeaponCasing, SpawnTM, SpawnParams);
}
