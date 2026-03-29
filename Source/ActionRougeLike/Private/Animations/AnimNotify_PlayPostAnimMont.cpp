// Copyright (c) 2026 Dylan. Personal Game Project. \nThis code is provided as-is for development and experimentation.\nUnauthorized use, distribution, or modification is not permitted.


#include "Animations/AnimNotify_PlayPostAnimMont.h"
#include "Player/SCharacter.h"

void UAnimNotify_PlayPostAnimMont::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("Custom AnimNotify triggered!"));
	}

	//need to somehow get a reference to the current ASCharacter* Character 
	AActor* Instigator = MeshComp->GetOwner();
	if (Instigator)
	{
		ASCharacter* Character = Cast<ASCharacter>(Instigator);
		if (Character && PlayNewMonteage)
		{

			//play montage?
			Character->PlayAnimMontage(PlayNewMonteage);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Failed access of character or PlayNewMonteage. Verify variables and/or Instigator/anim issue... [Class: %s]"), *GetNameSafe(MeshComp->GetOwner()));
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed access of aactor. Instigator issue... [Class: %s]"), *GetNameSafe(MeshComp->GetOwner()));
	}
}
