// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// 
// Unauthorized use, distribution, or modification is not permitted.

#include "Weapons/SBaseWeapon.h"
#include "Items/UItemBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"

// Sets default values
USBaseWeapon::USBaseWeapon()
{


}

/// if player hits attack and we have ammo spend ammo and attack
/// 
/// spawn projectile? For base class we don't have magazine so pull from straight ammo reserves


//think of these attacks as a provider of action
// like each weapon has an action of firing tied to it
//for example this base class could have the USAction_ProjectileAttack tied to it
//so when the player does a primary attacks we go through the inventory, to the selected weapon for 
//and runs the selected action (StartAction)

//ideal architecture: player hits button -> runs primaryattack function
// -> start action("PrimaryAttack") -> ActionComponent function for primary attack
// -> currently equiped weapon has attack function

//action should look up weapon not the other way around

//weapon should own the weapon specific animation

//action comp shouldn't care what weapon is equipped, doesn't care why its firing
//player char doesn't need to know anything besides i have a weapon equipped


//note: want to be able to add weapons without touching the action comp
// can add new actions without touching the weapon system
// make sure AI can use same weapons
// ideally want weapons to be tuned separately 
//want animations for crazy ass weapons to also be stored independently 
//the projectile attack fireball action class handles everything, may want to split it up

//make new USAction class that incorporates currently equipped weapon
//weapon will now handle ProjectileClass, SpawnSocketName (where projectile spawns - since it
// all depends on gun or weapon model), AttacAnimDelay, AttackAnim, AttackEffects

//should have a reference to player inventory more specifically the equipped weapon
//should be able to not just run the weapon firing sequence on ACharacter but also AICharacter

/// <summary>
/// When player swaps weapon from manager class, maybe a specific weapon behavior occurs? sound, reload sound could play, etc
/// </summary>
void USBaseWeapon::OnEquip()
{
	
}

/// <summary>
/// Sound plays when weapon is put away but animation should play from player animation class?
/// </summary>
void USBaseWeapon::Unequip()
{
	
}

/// <summary>
/// Check if we have ammo to reload and return
/// Magazine status is less than max mag size
/// Ammo to even reload
/// </summary>
/// <returns></returns>
bool USBaseWeapon::CanBeReloaded()
{
	return (CurrentMagazineSize < StandardMagazineSized && TotalAmmoReserves > StandardMagazineSized);
}


/// <summary>
/// refill the mag after reload animation finishes
/// </summary>
/// <returns></returns>
void USBaseWeapon::PerformReloadBehavior()
{
	if(TotalAmmoReserves > StandardMagazineSized)
	{ 
		TotalAmmoReserves = TotalAmmoReserves - StandardMagazineSized;
		CurrentMagazineSize = StandardMagazineSized;
	}
	else {
		CurrentMagazineSize = TotalAmmoReserves;
		TotalAmmoReserves = 0;
	}
}

/// <summary>
/// Moved the attack behavior from the action to be called directly here
/// </summary>
/// <param name="Instigator"></param>
void USBaseWeapon::PerformAttack(AActor* Instigator, USAction_WeaponAttack* OwningAttackAction)
{
	PlayAttackAnimation(Instigator);
	SpawnProjectile(Instigator);
	SpawnCasing(Instigator);
	PlayMuzzleFx(Instigator);

	CurrentMagazineSize--;

	FTimerHandle TimerHandle_AttackAnimDelay;
	FTimerDelegate DelegateAnimationDelay;
	DelegateAnimationDelay.BindUFunction(OwningAttackAction, "OnWeaponAttackFinished", Instigator);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackAnimDelay, DelegateAnimationDelay, AttacAnimDelay, false);
}

void USBaseWeapon::SpawnProjectile(AActor* Instigator)
{
	if (ensureAlways(WeaponProjectile))
	{
		//use weapons projectiles name instead of 'muzzle'
		UStaticMeshComponent* EquipedWeaponStaticMesh = GetItemStaticMesh();
		const FVector HandLocation = EquipedWeaponStaticMesh->GetSocketLocation(WeaponMuzzleSocketName);

		//EPSCPoolMethod PoolingMethod; //defaults to none
		//attach location can be KeepWorldPosition or KeepRelativeOffset
		// scale shouldn't be needed if we use keepworldposition

		//have particle effect in players hand when shooting projectile
		//should be where our hand socket is, where the projectile spawns in
		// cast root component as a scene component?
		//UParticleSystem* EmitterTemplate, USceneComponent* AttachToComponent, FName AttachPointName, FVector Location, FRotator Rotation, FVector Scale, EAttachLocation::Type LocationType, bool bAutoDestroy, EPSCPoolMethod PoolingMethod, bool bAutoActivateSystem
		// instructor used CastSpellVFX, GetMesh,HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true, EPSCPoolMethod::None, true);
		//UGameplayStatics::SpawnEmitterAttached(CastSpellVFX, GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
		//spawn particle effect from hand socket on mesh

		/*UGameplayStatics::SpawnEmitterAttached(
			CastingEffects,
			EquipedWeaponStaticMesh,
			WeaponMuzzleSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget
		);*/

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		APawn* PawnInstigator = Cast<APawn>(Instigator);
		SpawnParams.Instigator = PawnInstigator;


		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Instigator);

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);


		//for trace
		float basicTraceRange = 5000;

		//the crosshair is at the forward vector of camera so might as well just use that
		FVector TraceStart = PawnInstigator->GetPawnViewLocation();//CameraComp->GetComponentLocation();

		//endpoint far into the look at distance (not to far, still somewhat towards crosshair on a miss)
		FVector TraceEnd = TraceStart + (PawnInstigator->GetControlRotation().Vector() * basicTraceRange);

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
		AActor* T = GetWorld()->SpawnActor<AActor>(WeaponProjectile, SpawnTM, SpawnParams);
	}
}

/// <summary>
/// Ideally the ai should be able to play the attack montage as well. Make sure this works
/// </summary>
/// <param name="Instigator"></param>
void USBaseWeapon::PlayAttackAnimation(AActor* Instigator)
{
	ACharacter* InstigatorCharacter = Cast< ACharacter>(Instigator);
	if(!InstigatorCharacter) //not character?
		return;

	UAnimInstance* AnimInstance = InstigatorCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !AttackAnim)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant get animinstance. Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
		return;
	}


	InstigatorCharacter->PlayAnimMontage(AttackAnim);
}

void USBaseWeapon::SpawnCasing(AActor* Instigator)
{
	UStaticMeshComponent* EquipedWeaponStaticMesh = GetItemStaticMesh();
	const FVector CastingLocation = EquipedWeaponStaticMesh->GetSocketLocation("BulletEject");
	const FRotator CastingRotation = EquipedWeaponStaticMesh->GetSocketRotation("BulletEject");
	const FTransform SpawnTM = FTransform(CastingLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APawn* PawnInstigator = Cast<APawn>(Instigator);
	SpawnParams.Instigator = PawnInstigator;

	AAEjectedBulletCasing* casing = GetWorld()->SpawnActor<AAEjectedBulletCasing>(EjectedCasingActor, SpawnTM, SpawnParams);

	//eject it in a motion with impulse
	UStaticMeshComponent* casingSM = casing->CasingStaticMesh;
	FVector EjectDir = CastingRotation.Vector();
	casingSM->AddImpulse(EjectDir * casing->EjectionStrength, NAME_None, true);
}

void USBaseWeapon::PlayMuzzleFx(AActor* Instigator)
{
	UStaticMeshComponent* EquipedWeaponStaticMesh = GetItemStaticMesh();
	UGameplayStatics::SpawnEmitterAttached(
		CastingEffects,
		EquipedWeaponStaticMesh,
		WeaponMuzzleSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget
	);
}

/// <summary>
/// This solves the dependency issue!!
/// </summary>
/// <returns></returns>
EWeaponType USBaseWeapon::GetWeaponType_Implementation() const
{
	return WeaponType;
}

/// <summary>
/// Is a weapon, therefore is equippable
/// </summary>
/// <returns></returns>
bool USBaseWeapon::IsEquippable_Implementation_Implementation() const
{
	return true;
}

