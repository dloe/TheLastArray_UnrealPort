// Fill out your copyright notice in the Description page of Project Settings.

#include "Enviornment/TileGeneration/STile.h"
#include "STileDoorWallConnection.h"
#include "Engine/World.h"

// Sets default values
ASTile::ASTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TilesRoot"));
	RootComponent = DoorsRoot;
	//DoorsRoot->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASTile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASTile::SetUpDoorTransforms_Implementation()
{

}


//if neighbor is null, add a wall - specifically for tiles next to null spaces
void ASTile::ActivateWalls()
{

}

bool ASTile::IsBossTile() const
{
	return TileStatus == ETileStatus::ETile_BOSSROOM;
}

bool ASTile::IsStartingTile() const
{
	return TileStatus == ETileStatus::ETile_STARTINGROOM;
}

//if tile is starting, boss, secret
bool ASTile::IsNotSpecialTile()
{
	if (TileStatus != ETileStatus::ETile_BOSSROOM && TileStatus != ETileStatus::ETile_SECRETROOM && TileStatus != ETileStatus::ETile_STARTINGROOM)
	{
		//UE_LOG(LogTemp, Log, TEXT("Check12"));
		return true;
	}
	else
		return false;
}

#pragma region Door Setup



void ASTile::ActivateDoorToPath()
{

}

void ASTile::ActivateDoorsBranch()
{

}

void ASTile::ActivateDoorsRandom()
{

}

void ASTile::SyncDoors()
{

}

#pragma endregion

#pragma region Labeling Rooms

void ASTile::ShadeNull()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Red, false, 100);
	TileStatus = ETileStatus::ETile_NULLROOM;
}

void ASTile::ShadePath()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Blue, false, 100);

	TileStatus = ETileStatus::ETile_PATH;
}

void ASTile::ShadeActiveRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Silver, false, 100);
	TileStatus = ETileStatus::ETile_ROOM;
}

void ASTile::ShadeBossRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Emerald, false, 100);
	TileStatus = ETileStatus::ETile_BOSSROOM;
}

void ASTile::ShadeStartingRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Green, false, 100);
	TileStatus = ETileStatus::ETile_STARTINGROOM;
}

void ASTile::ShadeSecretRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Purple, false, 100);
	TileStatus = ETileStatus::ETile_SECRETROOM;
}

void ASTile::ShadeTestRoom()
{
	//DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Magenta, false, 100);
	//TileStatus = ETileStatus::ETile_ROOM;
}

void ASTile::ShadeEndRoom()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), 200.0f, 20, FColor::Black, false, 100);
	this->SetActorLabel("Tile_FinalRoom");
	TileStatus = ETileStatus::ETile_BOSSROOM;
}

bool ASTile::HasValidRightNeighbor()
{
	bool result = false;
	if (RightNeighbor && RightNeighbor->TileStatus == ETileStatus::ETile_NULLROOM)
	{
		result = true;
	}
	return result;
}

bool ASTile::HasValidLeftNeighbor()
{
	bool result = false;
	if (LeftNeighbor && LeftNeighbor->TileStatus == ETileStatus::ETile_NULLROOM)
	{
		result = true;
	}
	return result;
}

bool ASTile::HasValidDownNeighbor()
{
	bool result = false;
	if (DownNeighbor && DownNeighbor->TileStatus == ETileStatus::ETile_NULLROOM)
	{
		result = true;
	}
	return result;
}

bool ASTile::HasValidUpNeighbor()
{
	bool result = false;
	if (UpNeighbor && UpNeighbor->TileStatus == ETileStatus::ETile_NULLROOM)
	{
		result = true;
	}
	return result;
}

//we want a valid right neighbor, and a tile status that could be activated and attached to main path or some room
bool ASTile::HasConnectedRightNeighbor()
{
	bool result = false;

	if (RightNeighbor != NULL && (RightNeighbor->TileStatus == ETileStatus::ETile_PATH || RightNeighbor->TileStatus == ETileStatus::ETile_ROOM))
	{
		result = true;
	}

	return result;
}

bool ASTile::HasConnectedLeftNeighbor()
{
	bool result = false;

	if (LeftNeighbor != NULL && (LeftNeighbor->TileStatus == ETileStatus::ETile_PATH || LeftNeighbor->TileStatus == ETileStatus::ETile_ROOM))
	{
		result = true;
	}

	return result;
}

bool ASTile::HasConnectedUpNeighbor()
{
	//bool result = false;

	if (UpNeighbor != NULL && (UpNeighbor->TileStatus == ETileStatus::ETile_PATH || UpNeighbor->TileStatus == ETileStatus::ETile_ROOM))
	{
		//UE_LOG(LogTemp, Log, TEXT("Check4"));
		return true;
	}

	return false;
}

bool ASTile::HasConnectedDownNeighbor()
{
	bool result = false;

	if (DownNeighbor != NULL && (DownNeighbor->TileStatus == ETileStatus::ETile_PATH || DownNeighbor->TileStatus == ETileStatus::ETile_ROOM))
	{
		result = true;
	}

	return result;
}

void ASTile::TurnAllDoorsInactive()
{
	if (HasValidDownNeighbor())
		DownDoor->DoorActive = false;
	if (HasValidUpNeighbor())
		UpDoor->DoorActive = false;
	if (HasValidRightNeighbor())
		RightDoor = false;
	if (HasValidLeftNeighbor())
		LeftDoor = false;
}

//should i have more checks in here?
void ASTile::ConnectUpDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	UpDoor->DoorActive = true;
	RemoveCurrentWall(ETileSide::ETile_Up);
	//spawn door wall asset
	SpawnDoorConnector(ETileSide::ETile_Up, ChoosenDoorwayAsset, WallsSubFolderName);

}

void ASTile::ConnectDownDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	DownDoor->DoorActive = true;
	RemoveCurrentWall(ETileSide::ETile_Down);
	SpawnDoorConnector(ETileSide::ETile_Down, ChoosenDoorwayAsset, WallsSubFolderName);
}

void ASTile::ConnectLeftDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	LeftDoor->DoorActive = true;
	RemoveCurrentWall(ETileSide::ETile_Left);
	SpawnDoorConnector(ETileSide::ETile_Left, ChoosenDoorwayAsset, WallsSubFolderName);
}

void ASTile::ConnectRightDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	RightDoor->DoorActive = true;
	RemoveCurrentWall(ETileSide::ETile_Right);
	SpawnDoorConnector(ETileSide::ETile_Right, ChoosenDoorwayAsset, WallsSubFolderName);
}

void ASTile::ActivateUpDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	if (HasValidUpNeighbor()) {
		UpDoor->DoorActive = true;
		RemoveCurrentWall(ETileSide::ETile_Up);
		//
		//UpNeighbor->RemoveCurrentWall(ETileSide::ETile_Up);
		SpawnDoorConnector(ETileSide::ETile_Down, ChoosenDoorwayAsset, WallsSubFolderName);
	}
}

void ASTile::ActivateDownDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	if (HasValidDownNeighbor()) {
		DownDoor->DoorActive = true;
		RemoveCurrentWall(ETileSide::ETile_Down);
		SpawnDoorConnector(ETileSide::ETile_Down, ChoosenDoorwayAsset, WallsSubFolderName);
	}
}

void ASTile::ActivateRightDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	if (HasValidRightNeighbor()) {
		RightDoor->DoorActive = true;
		RemoveCurrentWall(ETileSide::ETile_Right);
		SpawnDoorConnector(ETileSide::ETile_Right, ChoosenDoorwayAsset, WallsSubFolderName);
	}
}

void ASTile::ActivateLeftDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	if (HasValidLeftNeighbor()) {
		LeftDoor->DoorActive = true;
		RemoveCurrentWall(ETileSide::ETile_Left);
		SpawnDoorConnector(ETileSide::ETile_Left, ChoosenDoorwayAsset, WallsSubFolderName);
	}
}

/// <summary>
/// Remove Given wall
/// </summary>
/// <param name="side"></param>
void ASTile::RemoveCurrentWall(ETileSide side)
{
	ASTileWall* wall;
	switch (side)
	{
	case ETileSide::ETile_Up:
		wall = UpWall;
		break;
	case ETileSide::ETile_Down:
		wall = DownWall;
		break;
	case ETileSide::ETile_Left:
		wall = LeftWall;
		break;
	case ETileSide::ETile_Right:
		wall = RightWall;
		break;
	default:
		wall = UpWall;
		UE_LOG(LogTemp, Error, TEXT("Default param for RemovePlaceholderWall"));
		break;

	}

	wall->Destroy();
}

/// <summary>
/// For spawning doorways for each door
/// </summary>
/// <param name="side"></param>
/// <param name="ThisTile"></param>
/// <param name="ChoosenDoorwayAsset"></param>
void ASTile::SpawnDoorConnector(ETileSide side, TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName)
{
	FVector WallLocation;
	FTransform WallSpawnTrans;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	switch (side)
	{
	case ETileSide::ETile_Up:
		WallLocation = SM_UpWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_UpWallSpawnPoint.GetRotation(), WallLocation);
		UpWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		UpWall->InnerTile = this;
		UpWall->OuterTile = UpNeighbor;

		UpWall->SetOwner(this);
#if WITH_EDITOR
		UpWall->SetFolderPath(WallsSubFolderName);
#endif
		break;
	case ETileSide::ETile_Down:
		WallLocation = SM_DownWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_DownWallSpawnPoint.GetRotation(), WallLocation);
		DownWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		DownWall->InnerTile = this;
		DownWall->OuterTile = DownNeighbor;

		DownWall->SetOwner(this);
#if WITH_EDITOR
		DownWall->SetFolderPath(WallsSubFolderName);
#endif
		break;
	case ETileSide::ETile_Left:
		WallLocation = SM_LeftWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_LeftWallSpawnPoint.GetRotation(), WallLocation);
		LeftWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		LeftWall->InnerTile = this;
		LeftWall->OuterTile = LeftNeighbor;

		LeftWall->SetOwner(this);
#if WITH_EDITOR
		LeftWall->SetFolderPath(WallsSubFolderName);
#endif
		break;
	case ETileSide::ETile_Right:
		WallLocation = SM_RightWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_RightWallSpawnPoint.GetRotation(), WallLocation);
		RightWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		RightWall->InnerTile = this;
		RightWall->OuterTile = RightNeighbor;

		RightWall->SetOwner(this);
#if WITH_EDITOR
		RightWall->SetFolderPath(WallsSubFolderName);
#endif
		break;
	default: //defaults to UP
		WallLocation = SM_UpWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_UpWallSpawnPoint.GetRotation(), WallLocation);
		UpWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		UpWall->InnerTile = this;
		UpWall->OuterTile = UpNeighbor;

		UpWall->SetOwner(this);
#if WITH_EDITOR
		UpWall->SetFolderPath(WallsSubFolderName);
#endif
		UE_LOG(LogTemp, Error, TEXT("Default param for RemovePlaceholderWall"));
		break;

	}



}

#pragma endregion



