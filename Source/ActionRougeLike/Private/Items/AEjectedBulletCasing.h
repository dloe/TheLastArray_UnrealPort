// Copyright (c) 2026 Dylan. 
// Personal Game Project. 
// 
// 
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AEjectedBulletCasing.generated.h"

UCLASS()
class AAEjectedBulletCasing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAEjectedBulletCasing();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Casing Info")
	int EjectionStrength = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Casing Info")
	UStaticMeshComponent* CasingStaticMesh;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
