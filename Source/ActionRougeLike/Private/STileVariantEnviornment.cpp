// Fill out your copyright notice in the Description page of Project Settings.

#include "STileVariantEnviornment.h"
#include "Enviornment/TileGeneration/STileDoor.h"
#include "Enviornment/TileGeneration/STile.h"
//#include "SFTileVariantDefinitionData.h"

// Sets default values
ASTileVariantEnviornment::ASTileVariantEnviornment()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASTileVariantEnviornment::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTileVariantEnviornment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//destroys all component markers (for level enemy/asset placement)
void ASTileVariantEnviornment::HandleMarkerCleanup()
{
	//remove pickup markers
	for (UStaticMeshComponent* PickupMarker : PickupPlacements)
	{
		PickupMarker->DestroyComponent();
	}
	PickupPlacements.Empty();

	//remove enemy markers
	for (UStaticMeshComponent* EnemyMarker : EnemyPlacements)
	{
		EnemyMarker->DestroyComponent();
	}
	EnemyPlacements.Empty();

	//remove obj markers
	for (UStaticMeshComponent* ObjMarker : ObjectivePlacements)
	{
		ObjMarker->DestroyComponent();
	}
	ObjectivePlacements.Empty();


}

