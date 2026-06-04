// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.


#include "Animations/AnimNotify_FootstepNotify.h"
#include "GameFramework/Actor.h"
#include "Player/SCharacter.h"

void UAnimNotify_FootstepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//check owner
	/*if (APawn* OwningPawn = Cast<APawn>(MeshComp->GetOwner()))
	{
		const float Loudness = 0.3f;
		const float MaxRange = 600;

		OwningPawn->MakeNoise(Loudness, OwningPawn, OwningPawn->GetActorLocation(), MaxRange);
	}*/
	if (ASCharacter* Character = Cast<ASCharacter>(MeshComp->GetOwner()))
	{
		Character->HandleFootstep();
	}
}
