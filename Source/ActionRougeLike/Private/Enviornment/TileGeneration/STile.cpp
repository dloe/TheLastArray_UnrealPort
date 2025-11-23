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


///if neighbor is null, add a wall - specifically for tiles next to null space
void ASTile::ActivateWalls(TSubclassOf<ASTileWall> ChoosenWallAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	FVector WallLocation;
	FTransform WallSpawnTrans;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//if down neighbor is null and down wall null 
	if (DownNeighbor == NULL && DownWall == NULL)
	{
		WallLocation = SM_DownWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_DownWallSpawnPoint.GetRotation(), WallLocation);
		DownWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		DownWall->InnerTile = this;
		DownWall->RemoveOuter();
		AllSpawnedWalls.Add(DownWall);

		DownWall->SetOwner(this);
#if WITH_EDITOR
		DownWall->SetFolderPath(WallsSubFolderName);
#endif
	}
	else if (DownNeighbor != NULL && DownWall == NULL)
	{
		DownWall = DownNeighbor->UpWall;
	}

	//Up wall
	if (UpNeighbor == NULL && UpWall == NULL)
	{
		WallLocation = SM_UpWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_UpWallSpawnPoint.GetRotation(), WallLocation);
		UpWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		UpWall->InnerTile = this;
		UpWall->RemoveOuter();
		AllSpawnedWalls.Add(UpWall);

		UpWall->SetOwner(this);
#if WITH_EDITOR
		UpWall->SetFolderPath(WallsSubFolderName);
#endif
	}
	else if (UpNeighbor != NULL && UpWall == NULL)
	{
		UpWall = UpNeighbor->DownWall;
	}

	//left wall
	if (LeftNeighbor == NULL && LeftWall == NULL)
	{
		WallLocation = SM_LeftWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_LeftWallSpawnPoint.GetRotation(), WallLocation);
		LeftWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		LeftWall->InnerTile = this;
		LeftWall->RemoveOuter();
		AllSpawnedWalls.Add(LeftWall);

		LeftWall->SetOwner(this);
#if WITH_EDITOR
		LeftWall->SetFolderPath(WallsSubFolderName);
#endif
	}
	else if (LeftNeighbor != NULL && LeftWall == NULL)
	{
		LeftWall = LeftNeighbor->RightWall;
	}

	//right wall
	if (RightNeighbor == NULL && RightWall == NULL)
	{
		WallLocation = SM_RightWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_RightWallSpawnPoint.GetRotation(), WallLocation);
		RightWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		RightWall->InnerTile = this;
		RightWall->RemoveOuter();
		AllSpawnedWalls.Add(RightWall);

		RightWall->SetOwner(this);
#if WITH_EDITOR
		RightWall->SetFolderPath(WallsSubFolderName);
#endif
	}
	else if (RightNeighbor != NULL && RightWall == NULL)
	{
		RightWall = RightNeighbor->LeftWall;
	}

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
	if (UpNeighbor != NULL && (UpNeighbor->TileStatus == ETileStatus::ETile_PATH || UpNeighbor->TileStatus == ETileStatus::ETile_ROOM))
	{
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

/// <summary>
/// Turn all doors off that exist. No connections to this tile, quarentine.
/// </summary>
void ASTile::TurnAllDoorsInactive()
{
	//compiller tries to opimize calls and for some reason, calling the hasvalid functions directly in the 
	// if statements cause it to skip. This approach will get the values we need, then use those as needed. Hopefully...
	//research shows this is common in developer and debug game editor

	//UE_LOG(LogTemp, Log, TEXT("test %d,%d,%d,%d"), DownNeighborValid, UpNeighborValid, RightNeighborValid, LeftNeighborValid);
	if (DownNeighbor != NULL && DownDoor != NULL)
		DownDoor->DoorActive = false;
	if (UpNeighbor != NULL && UpDoor != NULL)
		UpDoor->DoorActive = false;
	if (RightNeighbor != NULL && RightDoor != NULL)
		RightDoor->DoorActive = false;
	if (LeftNeighbor != NULL && LeftDoor != NULL)
		LeftDoor->DoorActive = false;
}

//should i have more checks in here?
void ASTile::ConnectUpDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	UpDoor->DoorActive = true;
	RemoveCurrentWall(ETileSide::ETile_Up);
	//spawn door wall asset
	if (!UpDoor->DestroyConnectorWalls && UpDoor->DoorsConnector == NULL)
		UpDoor->DoorsConnector = SpawnDoorConnector(ETileSide::ETile_Up, ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);

}

void ASTile::ConnectDownDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	DownDoor->DoorActive = true;
	RemoveCurrentWall(ETileSide::ETile_Down);
	if(!DownDoor->DestroyConnectorWalls && DownDoor->DoorsConnector == NULL)
		DownDoor->DoorsConnector = SpawnDoorConnector(ETileSide::ETile_Down, ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
}

void ASTile::ConnectLeftDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	LeftDoor->DoorActive = true;
	RemoveCurrentWall(ETileSide::ETile_Left);
	if (!LeftDoor->DestroyConnectorWalls && LeftDoor->DoorsConnector == NULL)
		LeftDoor->DoorsConnector = SpawnDoorConnector(ETileSide::ETile_Left, ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
}

void ASTile::ConnectRightDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	RightDoor->DoorActive = true;
	RemoveCurrentWall(ETileSide::ETile_Right);
	if (!RightDoor->DestroyConnectorWalls && RightDoor->DoorsConnector == NULL)
		RightDoor->DoorsConnector = SpawnDoorConnector(ETileSide::ETile_Right, ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
}

void ASTile::ActivateUpDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	if (HasValidUpNeighbor()) {
		UpDoor->DoorActive = true;
		RemoveCurrentWall(ETileSide::ETile_Up);
		if (!UpDoor->DestroyConnectorWalls && UpDoor->DoorsConnector == NULL)
			UpDoor->DoorsConnector = SpawnDoorConnector(ETileSide::ETile_Up, ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
	}
}

void ASTile::ActivateDownDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	if (HasValidDownNeighbor()) {
		DownDoor->DoorActive = true;
		RemoveCurrentWall(ETileSide::ETile_Down);
		if (!DownDoor->DestroyConnectorWalls && DownDoor->DoorsConnector == NULL)
			DownDoor->DoorsConnector = SpawnDoorConnector(ETileSide::ETile_Down, ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
	}
}

void ASTile::ActivateRightDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	if (HasValidRightNeighbor()) {
		RightDoor->DoorActive = true;
		RemoveCurrentWall(ETileSide::ETile_Right);
		if (!RightDoor->DestroyConnectorWalls && RightDoor->DoorsConnector == NULL)
			RightDoor->DoorsConnector = SpawnDoorConnector(ETileSide::ETile_Right, ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
	}
}

void ASTile::ActivateLeftDoor(TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	if (HasValidLeftNeighbor()) {
		LeftDoor->DoorActive = true;
		RemoveCurrentWall(ETileSide::ETile_Left);
		if (!LeftDoor->DestroyConnectorWalls && LeftDoor->DoorsConnector == NULL)
			LeftDoor->DoorsConnector = SpawnDoorConnector(ETileSide::ETile_Left, ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
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
	if(wall != NULL)
		wall->Destroy();
	else {
		UE_LOG(LogTemp, Log, TEXT("Wall is null for %s"), *GetNameSafe(this));
	}
}

/// <summary>
/// For spawning doorways for each door
/// </summary>
/// <param name="side"></param>
/// <param name="ThisTile"></param>
/// <param name="ChoosenDoorwayAsset"></param>
ASTileWall* ASTile::SpawnDoorConnector(ETileSide side, TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	FVector WallLocation;
	FTransform WallSpawnTrans;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ASTileWall* SpawnedDoorWallConnector;

	FString ConnectorName = "TileConnector_";
	
	switch (side)
	{
	case ETileSide::ETile_Up:
		WallLocation = SM_UpWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_UpWallSpawnPoint.GetRotation(), WallLocation);
		UpWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		UpWall->InnerTile = this;
		UpWall->OuterTile = UpNeighbor;
		ConnectorName += FString::FromInt(this->XIndex) + "_" + FString::FromInt(this->ZIndex) + "-" + FString::FromInt(UpNeighbor->XIndex) + "_" + FString::FromInt(UpNeighbor->ZIndex);
		AllSpawnedWalls.Add(UpWall);
		UpNeighbor->RemoveCurrentWall(ETileSide::ETile_Down);
		UpWall->SetOwner(this);
#if WITH_EDITOR
		UpWall->SetFolderPath(WallsSubFolderName);
#endif
		SpawnedDoorWallConnector = UpWall;
		break;
	case ETileSide::ETile_Down:
		WallLocation = SM_DownWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_DownWallSpawnPoint.GetRotation(), WallLocation);
		DownWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		DownWall->InnerTile = this;
		DownWall->OuterTile = DownNeighbor;
		ConnectorName += FString::FromInt(this->XIndex) + "_" + FString::FromInt(this->ZIndex) + "-" + FString::FromInt(DownNeighbor->XIndex) + "_" + FString::FromInt(DownNeighbor->ZIndex);
		AllSpawnedWalls.Add(DownWall);
		DownNeighbor->RemoveCurrentWall(ETileSide::ETile_Up);
		DownWall->SetOwner(this);
#if WITH_EDITOR
		DownWall->SetFolderPath(WallsSubFolderName);
#endif
		SpawnedDoorWallConnector = DownWall;
		break;
	case ETileSide::ETile_Left:
		WallLocation = SM_LeftWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_LeftWallSpawnPoint.GetRotation(), WallLocation);
		LeftWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		LeftWall->InnerTile = this;
		LeftWall->OuterTile = LeftNeighbor;
		ConnectorName += FString::FromInt(this->XIndex) + "_" + FString::FromInt(this->ZIndex) + "-" + FString::FromInt(LeftNeighbor->XIndex) + "_" + FString::FromInt(LeftNeighbor->ZIndex);
		AllSpawnedWalls.Add(LeftWall);
		LeftNeighbor->RemoveCurrentWall(ETileSide::ETile_Right);
		LeftWall->SetOwner(this);
#if WITH_EDITOR
		LeftWall->SetFolderPath(WallsSubFolderName);
#endif
		SpawnedDoorWallConnector = LeftWall;
		break;
	case ETileSide::ETile_Right:
		WallLocation = SM_RightWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_RightWallSpawnPoint.GetRotation(), WallLocation);
		RightWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		RightWall->InnerTile = this;
		RightWall->OuterTile = RightNeighbor;
		ConnectorName += FString::FromInt(this->XIndex) + "_" + FString::FromInt(this->ZIndex) + "-" + FString::FromInt(RightNeighbor->XIndex) + "_" + FString::FromInt(RightNeighbor->ZIndex);
		AllSpawnedWalls.Add(RightWall);
		RightNeighbor->RemoveCurrentWall(ETileSide::ETile_Left);
		RightWall->SetOwner(this);
#if WITH_EDITOR
		RightWall->SetFolderPath(WallsSubFolderName);
#endif
		SpawnedDoorWallConnector = RightWall;
		break;
	default: //defaults to UP
		WallLocation = SM_UpWallSpawnPoint.GetLocation() + GetActorLocation();
		WallSpawnTrans = FTransform(SM_UpWallSpawnPoint.GetRotation(), WallLocation);
		UpWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenDoorwayAsset, WallSpawnTrans, SpawnParams);
		UpWall->InnerTile = this;
		UpWall->OuterTile = UpNeighbor;
		ConnectorName += FString::FromInt(this->XIndex) + "_" + FString::FromInt(this->ZIndex) + "-" + FString::FromInt(UpNeighbor->XIndex) + "_" + FString::FromInt(UpNeighbor->ZIndex);
		AllSpawnedWalls.Add(UpWall);
		UpNeighbor->RemoveCurrentWall(ETileSide::ETile_Down);
		UpWall->SetOwner(this);
#if WITH_EDITOR
		UpWall->SetFolderPath(WallsSubFolderName);
#endif
		UE_LOG(LogTemp, Error, TEXT("Default param for RemovePlaceholderWall"));
		SpawnedDoorWallConnector = UpWall;
		break;
	}
	SpawnedDoorWallConnector->SetActorLabel(ConnectorName);

	return SpawnedDoorWallConnector;
}

/// <summary>
/// setup for secret room
/// </summary>
/// <param name="side"></param>
/// <param name="SecretRoomAsset"></param>
/// <param name="ChoosenWallAsset"></param>
/// <param name="WallsSubFolderName"></param>
/// <param name="AllSpawnedWalls"></param>
void ASTile::SetupSecretRoomDoorWalls(ETileSide side, TSubclassOf<ASTileWall> ChoosenWallAsset, FName WallsSubFolderName, TArray<ASTileWall*>& AllSpawnedWalls)
{
	FTransform WallSpawnTrans;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (int index = 0; index < 4; index++)
	{
		FVector WallLocation;
		ASTileWall* currentWall = nullptr;
		ASTile* Neighbor = nullptr;
		if (side != ETileSide::ETile_Up)
		{
			if (UpNeighbor)
			{
				UpWall = UpNeighbor->DownWall;
			}
			else {
				WallLocation = SM_UpWallSpawnPoint.GetLocation() + GetActorLocation();
				WallSpawnTrans = FTransform(SM_UpWallSpawnPoint.GetRotation(), WallLocation);
				currentWall = UpWall;
				Neighbor = UpNeighbor;
			}
		}
		else if (side != ETileSide::ETile_Down)
		{
			if (DownNeighbor)
			{
				DownWall = DownNeighbor->UpWall;
			}
			else {
				WallLocation = SM_DownWallSpawnPoint.GetLocation() + GetActorLocation();
				WallSpawnTrans = FTransform(SM_DownWallSpawnPoint.GetRotation(), WallLocation);
				currentWall = DownWall;
				Neighbor = DownNeighbor;
			}
		}
		else if (side != ETileSide::ETile_Left)
		{
			if (LeftNeighbor)
			{
				LeftWall = LeftNeighbor->RightWall;
			}
			else {
				WallLocation = SM_LeftWallSpawnPoint.GetLocation() + GetActorLocation();
				WallSpawnTrans = FTransform(SM_LeftWallSpawnPoint.GetRotation(), WallLocation);
				currentWall = LeftWall;
				Neighbor = LeftNeighbor;
			}
		}
		else if (side != ETileSide::ETile_Right)
		{
			if (RightNeighbor)
			{
				RightWall = RightNeighbor->RightWall;
			}
			else {
				WallLocation = SM_RightWallSpawnPoint.GetLocation() + GetActorLocation();
				WallSpawnTrans = FTransform(SM_RightWallSpawnPoint.GetRotation(), WallLocation);
				currentWall = RightWall;
				Neighbor = RightNeighbor;
			}
		}

		if (IsValid(currentWall) && !WallSpawnTrans.Equals(FTransform::Identity) && !WallLocation.ContainsNaN()) {
			currentWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
			currentWall->InnerTile = this;
			currentWall->OuterTile = Neighbor;
			AllSpawnedWalls.Add(currentWall);

			currentWall->SetOwner(this);
#if WITH_EDITOR
			currentWall->SetFolderPath(WallsSubFolderName);
#endif
		}
	}
}

#pragma endregion



