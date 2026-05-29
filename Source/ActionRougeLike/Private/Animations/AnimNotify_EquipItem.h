// Copyright (c) 2026 Dylan. Personal Game Project. 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EquipItem.generated.h"

/**
 * 
 */
UCLASS()
class UAnimNotify_EquipItem : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(BlueprintReadWrite, Category = "Test")
	int Test;
};
