// Fill out your copyright notice in the Description page of Project Settings.


#include "SLocalLevel.h"

// Sets default values
ASLocalLevel::ASLocalLevel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASLocalLevel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASLocalLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

