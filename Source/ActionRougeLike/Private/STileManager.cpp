// Fill out your copyright notice in the Description page of Project Settings.


#include "STileManager.h"

// Sets default values
ASTileManager::ASTileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASTileManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

