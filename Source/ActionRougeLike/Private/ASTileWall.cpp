// Fill out your copyright notice in the Description page of Project Settings.

//#include "TileGeneration/STile.h"
#include "ASTileWall.h"

// Sets default values
ASTileWall::ASTileWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASTileWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTileWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASTileWall::RemoveOuter()
{
	//OuterWallObject->DestroyComponent();
	WillRemoveOuter = false;
}

void ASTileWall::RemoveInner()
{

}

