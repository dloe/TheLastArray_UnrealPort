// Copyright (c) 2026 Dylan. 
// Personal Game Project. 
// 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_PlayPostAnimMont.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API UAnimNotify_PlayPostAnimMont : public UAnimNotify
{
	GENERATED_BODY()
	
	public:
		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere,  Category = "Montage")
	UAnimMontage* PlayNewMonteage;
};
