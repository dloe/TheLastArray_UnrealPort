// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "Projectiles/SProjectileBase.h"

#include "Attributes/SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ASProjectileBase::ASProjectileBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	//SphereComp->OnComponentHit.__Internal_AddDynamic(this, &ASProjectileBase::OnActorHit);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MoveComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MoveComp->InitialSpeed = 1000.0f;
	MoveComp->bRotationFollowsVelocity = true;
	MoveComp->bInitialVelocityInLocalSpace = true;
	MoveComp->ProjectileGravityScale = 0.0f;

	AudioCompFlight = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioCompFlight->SetupAttachment(RootComponent);

	ImpactShakeInnerRadius = 0.0f;
	ImpactShakeOuterRadius = 1500.0f;

	//SetReplicates(true);
	bReplicates = true;
}

void ASProjectileBase::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//if (OtherActor != GetInstigator()) {
		Explode();
	//}
}

//implementation from it being marked as BlueprintNativeEvent
void ASProjectileBase::Explode_Implementation()
{
	//check to make sure we aren't already being destroyed
	//adding ensure to see if we counter this situation at all
	if (ensure(!this->IsActorBeingDestroyed()))//!IsValidChecked(this)) //future me: did we need that ensure here?
	{
		APawn* InstigatorPawn = GetInstigator();
		if (InstigatorPawn) {
			InstigatorPawn->MakeNoise(
				Loudness,
				InstigatorPawn,
				GetActorLocation(),
				MaxRange,
				TEXT("Projectile Explosion Impact")
			);
		}
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactVFX, GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation(), GetActorRotation());
		UGameplayStatics::PlayWorldCameraShake(this, MyShake, GetActorLocation(), ImpactShakeInnerRadius, ImpactShakeOuterRadius, Falloff);
		Destroy();
	}
}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//SphereComp->IgnoreActorWhenMoving(GetInstigator(), true);
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectileBase::OnActorHit);
}




