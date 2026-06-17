// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.


#include "Weapons/USFirearmWeapon.h"
#include "Kismet/GameplayStatics.h"

bool USFirearmWeapon::CanBeReloaded()
{
	return (CurrentMagazineSize < StandardMagazineSized && TotalAmmoReserves > StandardMagazineSized);
}

void USFirearmWeapon::PerformReloadStats()
{
	if (TotalAmmoReserves > StandardMagazineSized)
	{
		TotalAmmoReserves = TotalAmmoReserves - StandardMagazineSized;
		CurrentMagazineSize = StandardMagazineSized;
	}
	else {
		CurrentMagazineSize = TotalAmmoReserves;
		TotalAmmoReserves = 0;
	}
}

void USFirearmWeapon::PerformAttack(AActor* Instigator, USAction_WeaponAttack* OwningAttackAction)
{
	PlayAnimation(Instigator, AttackAnim);
	SpawnProjectile(Instigator);
	SpawnCasing(Instigator);
	PlayMuzzleFx(Instigator);

	CurrentMagazineSize--;

	if(Instigator->HasAuthority()) {
		FTimerHandle TimerHandle_AttackAnimDelay;
		FTimerDelegate DelegateAnimationDelay;
		DelegateAnimationDelay.BindUFunction(OwningAttackAction, "OnWeaponAttackFinished", Instigator);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackAnimDelay, DelegateAnimationDelay, AttacAnimDelay, false);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("PerformAttack: Instigator doesnt have authority. Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
	}
}

/// <summary>
/// Perform reload behavior
/// run anim, delay and then run
/// </summary>
/// <param name="Instigator"></param>
/// <param name="OwningReloadAction"></param>
void USFirearmWeapon::PerformReload(AActor* Instigator, USAction_WeaponReload* OwningReloadAction)
{
	PlayAnimation(Instigator, ReloadAnim);

	if (Instigator->HasAuthority()) {
		FTimerHandle TimerHandle_AttackDelay;
		FTimerDelegate Delegate;
		//unless there is a 'warm up' animation that has to run before we can fire the weapon or make the attack, this will most likely be near 0
		Delegate.BindUFunction(OwningReloadAction, "ReloadDelay_Elasped", Instigator);

		//when timer finishes, spawn projectile
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, ReloadAnimDelay, false);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("PerformReload: Instigator doesnt have authority. Instigator issue... [Class: %s]"), *GetNameSafe(Instigator));
	}
}

bool USFirearmWeapon::CanAttackWithWeapon()
{
	bool CanFireWeapon = false;

	//USBaseWeapon* EquipedWeaponFromInventory = Cast<USBaseWeapon>(EquippedItem);
	//reload if mag is less than max capacity
	if (CurrentMagazineSize > 0 &&
		CurrentMagazineSize <= StandardMagazineSized)
	{
		//hasWeaponEquipped = true;
		CanFireWeapon = true;
	}

	return CanFireWeapon;
}

void USFirearmWeapon::SpawnProjectile(AActor* Instigator)
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

		if (PawnInstigator) {
			PawnInstigator->MakeNoise(
				Loudness,
				PawnInstigator,
				PawnInstigator->GetActorLocation(),
				MaxRange,
				TEXT("Weapon Attack Sound")
			);
		}
	}
}

void USFirearmWeapon::SpawnCasing(AActor* Instigator)
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

void USFirearmWeapon::PlayMuzzleFx(AActor* Instigator)
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
