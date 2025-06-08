// Fill out your copyright notice in the Description page of Project Settings.


#include "UTileGridBranchComponent.h"
#include "Components/ActorComponent.h"
#include "Enviornment/TileGeneration/STileManager.h"

// Sets default values for this component's properties
UTileGridBranchComponent::UTileGridBranchComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	//PrimaryComponentTick.bCanEverTick = true;

	// ...
}

/// <summary>
/// This is called from STileManager after initial grid and path is setup.
/// 
/// Calls modules to handle random rooms, random branches, single rooms, secret rooms, 
/// final doors setup and deactivating inactive rooms
/// </summary>
void UTileGridBranchComponent::GameMapAdditionalSetup()
{
	TileManagerRef = Cast<ASTileManager>(GetOwner());
	LocalLevelRef = TileManagerRef->MyLocalLevel;

	UE_LOG(LogTemp, Log, TEXT("-----------------------------------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("========== Grid Additions and Final Setup ================="));
	UE_LOG(LogTemp, Log, TEXT("-----------------------------------------------------------"));

	RandomRoomsAndBranchesAdditions();

	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished Random Rooms - Adding Spawn Room =============================="));



	AddSingleRooms();

	if (LocalLevelRef->CurrentLevelTier >= ELevelTier::ELevel_2) {
		if (TileManagerRef->DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("=================== Finished Spawn Room - Adding Secret Room =============================="));
		CreateSecretRoom();

	}


	if (TileManagerRef->DoorsActive) {
		if (TileManagerRef->DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("=================== Finished Secret Room - Activating All Doors =============================="));

		FinalDoorSetupDoors();
	}

	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished All Doors - Implementing Final Tile Setup =============================="));

	DeactiveInactiveRooms();


	//notify next component to run
	OnGridAdditionalSetupCompletedEvent.Broadcast();
}


/// <summary>
/// Dylan Loe
/// 
/// - Adding Single random and branches to grid
/// </summary>
void UTileGridBranchComponent::RandomRoomsAndBranchesAdditions()
{
	int levelWidthRef = TileManagerRef->GetLevelWidth();
	int levelHeightRef = TileManagerRef->GetLevelHeight();


	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Adding Branches"));

	TileManagerRef->AllActiveTiles.Append(TileManagerRef->LevelPath);

	TileManagerRef->MakeAvailableTiles();

	//TODO: how long will branches be? Get better way to find this
	//Ideas/Research: 
	// - Adaptive Branch grown: iteratively grow branches based on available space, after/during each branch, check if we can still make more to a certain amount
	// - Controlled Density approach: define the number of branches based on total maze size and expected complexity
	//Controlled Density approach
	int TotalBranchesMax1 = FMath::RoundToInt(levelWidthRef * levelHeightRef * TileManagerRef->BranchDensityFactor_DynamicMainPathLength());
	//Dependency On maze size approach
	int TotalBranchesMax2 = FMath::RoundToInt(levelWidthRef * levelHeightRef * TileManagerRef->BranchDensityFactor_DependencyOnMazeSize());
	// - Directional Bias Control: weighted probability function, where branch creation probability decreases as the main path progresses

	int oldWay = (levelWidthRef - TileManagerRef->LevelPath.Num() / levelWidthRef) + 1;
	//for some randomness
	int TotalBranchesMax = TileManagerRef->GameStream.RandRange(1, TotalBranchesMax1);

	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Total amount of branches to create: %d"), TotalBranchesMax);

	for (int CurrentBranch = 0; CurrentBranch < TotalBranchesMax && TileManagerRef->AvailableTiles.Num() > 1; CurrentBranch++)
	{
		//for now using length of level, might change this later, not sure how else but not a super important detail
		int BranchLength = TileManagerRef->GameStream.RandRange(1, levelWidthRef);
		if (TileManagerRef->DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Making Branch: %d with length %d"), CurrentBranch, BranchLength);

		//pick random index that isn't boss tile
		//pick a valid neighbor that isn't part of path or outside grid, thats not a boss tile
		//this tile is now the start of a branch

		//starting tile for branch
		int indexChoosen = TileManagerRef->GameStream.RandRange(0, TileManagerRef->AvailableTiles.Num() - 1);
		ASTile* StartingBranchTile = TileManagerRef->AvailableTiles[indexChoosen];

		TArray<ASTile*>	BranchArray;

		//remove starting branch tile
		TileManagerRef->AvailableTiles.Remove(StartingBranchTile);

		//added StartingBranchTile to branch array for debug purposes
		int branchDoorConnectorSideCheck = CheckPathSide(StartingBranchTile);
		//UE_LOG(LogTemp, Log, TEXT("Check start branch %d: %d,%d on side %d"), CurrentBranch, StartingBranchTile->XIndex, StartingBranchTile->ZIndex, branchDoorConnectorSideCheck);
		//TODO: print out which tile and which side we went with

		CheckBranchTile(StartingBranchTile, BranchArray, BranchLength, branchDoorConnectorSideCheck);

		//run through branch
		for (int BranchIndex = 0; BranchIndex < BranchArray.Num(); BranchIndex++)
		{
			BranchArray[BranchIndex]->TileDescription += "Branch_" + FString::FromInt(CurrentBranch) + "";
			BranchArray[BranchIndex]->PathNumber = BranchIndex;

			if (!TileManagerRef->AllActiveTiles.Contains(BranchArray[BranchIndex]))
			{
				TileManagerRef->AllActiveTiles.AddUnique(BranchArray[BranchIndex]);
			}
			else if (TileManagerRef->AvailableTiles.Contains(BranchArray[BranchIndex]))
			{
				TileManagerRef->AvailableTiles.Remove(BranchArray[BranchIndex]);
			}

			if (BranchIndex == BranchArray.Num() - 1)
			{
				BranchArray[BranchIndex]->TileDescription += "ENDBRANCH";
				BranchArray[BranchIndex]->EndOfBranchPath = true;
			}
		}

		//once we make branch, we go back through and remake the available tile spots
		TileManagerRef->MakeAvailableTiles();

		//Debug draw branch
		if (TileManagerRef->DebugPrints) {

			//draw lines through path
			for (int Index = 0; Index < BranchArray.Num() - 1; Index++)
			{
				DrawDebugLine(GetWorld(), BranchArray[Index]->GetActorLocation(), BranchArray[Index + 1]->GetActorLocation(), FColor::Emerald, SDPG_World, 20.0f, 150);
			}
		}
	}
}

/// <summary>
/// Dylan Loe
/// 
/// - Implementing single chosen rooms to available tiles
/// </summary>
void UTileGridBranchComponent::AddSingleRooms()
{
	int levelWidthRef = TileManagerRef->GetLevelWidth();
	int levelHeightRef = TileManagerRef->GetLevelHeight();

	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Adding Single Rooms..."));
	//when we add a room, remove it from AvailableTiles, add to AllActiveTiles

	//default to half the rooms left over
	FillerRooms = TileManagerRef->GameStream.RandRange(1, (levelWidthRef - TileManagerRef->AvailableTiles.Num() - 1) - ((levelHeightRef - TileManagerRef->AvailableTiles.Num() - 1) / 4));
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Total Random Single Rooms: %d"), FillerRooms);

	for (int STileCount = 0; STileCount < FillerRooms; STileCount++)
	{
		//total tiles * density percentage
		//ensure that total tiles / used tiles doesn't exceed this percentage

		//if (LevelWidth - AllActiveTiles.Num() >= LevelHeight / (LevelWidth * 2))
		if (TileManagerRef->gridDensity >= TileManagerRef->GetCurrentGridDensity())
		{
			ASTile* Current = TileManagerRef->AvailableTiles[TileManagerRef->GameStream.RandRange(0, TileManagerRef->AvailableTiles.Num() - 1)];
			//UE_LOG(LogTemp, Log, TEXT("Room selected: %d:%d"), Current->XIndex, Current->ZIndex);
			if (Current->TileStatus == ETileStatus::ETile_NULLROOM)
			{
				Current->ShadeActiveRoom();
				TileManagerRef->AvailableTiles.Remove(Current);
				TileManagerRef->AllActiveTiles.AddUnique(Current);
				Current->TileDescription = "Random Single Room";

				SingleRoomsDoorSetup(Current);

				//Activate Doors
				//Remake new available list (with this currents neighbors now added
				//TO DO - Size small: Optimize the remake so that we only add the new tiles rooms instead of having to go through entire list

				TileManagerRef->MakeAvailableTiles();
			}
		}
		else
		{
			//stop adding random tiles
			return;
		}
	}
}

/// <summary>
/// Dylan Log
/// 
/// - Compose list of all null neighbors of all active tiles (excluding start and boss rooms)
/// - 
/// </summary>
void UTileGridBranchComponent::CreateSecretRoom()
{
	TArray<ASTile*> outskirtsCheck;
	for (int tileCount = 0; tileCount < TileManagerRef->AllActiveTiles.Num(); tileCount++)
	{
		ASTile* currentTile = TileManagerRef->AllActiveTiles[tileCount];

		FTileInfoStruct currentInfo;
		currentInfo.tile = currentTile;

		if (!currentTile->IsBossTile() && !currentTile->IsStartingTile())
		{
			//if up neighbor is a null ref
			if (!currentTile->UpNeighbor)
			{
				currentInfo.neighborArray.Add(1);
			}
			else if (currentTile->HasValidUpNeighbor() &&
				!currentTile->UpNeighbor->IsBossTile() && !currentTile->UpNeighbor->IsStartingTile())
			{
				currentInfo.neighborArray.Add(1);
			}

			//check left neighbor
			if (!currentTile->LeftNeighbor)
			{
				currentInfo.neighborArray.Add(3);
			}
			else if (currentTile->HasValidLeftNeighbor() &&
				!currentTile->LeftNeighbor->IsBossTile() && !currentTile->LeftNeighbor->IsStartingTile())
			{
				currentInfo.neighborArray.Add(3);
			}

			if (!currentTile->RightNeighbor)
			{
				currentInfo.neighborArray.Add(4);
			}
			else if (currentTile->HasValidRightNeighbor() &&
				!currentTile->RightNeighbor->IsBossTile() && !currentTile->RightNeighbor->IsStartingTile())
			{
				currentInfo.neighborArray.Add(4);
			}

			if (!currentTile->DownNeighbor)
			{
				currentInfo.neighborArray.Add(2);
			}
			else if (currentTile->HasValidDownNeighbor() &&
				!currentTile->DownNeighbor->IsBossTile() && !currentTile->DownNeighbor->IsStartingTile())
			{
				currentInfo.neighborArray.Add(2);
			}

			//add our info struct to list
			if (currentInfo.neighborArray.Num() != 0 && !outskirtsCheck.Contains(currentInfo.tile)) //OutskirtTiles.Contains(currentInfo))
			{
				TileManagerRef->OutskirtTiles.Add(currentInfo);
				outskirtsCheck.Add(currentInfo.tile);
			}
		}
	}

	//now randomly pick a tile to put our secret room at (this tiles neighbor will be the secret room)
	int tileNum = TileManagerRef->GameStream.RandRange(0, TileManagerRef->OutskirtTiles.Num() - 1);

	FTileInfoStruct selected = TileManagerRef->OutskirtTiles[tileNum];
	ASTile* test = outskirtsCheck[tileNum];
	choosen = outskirtsCheck[tileNum];

	//reshuffle our n value
	selected.neighborArray = TileManagerRef->Reshuffle2(selected.neighborArray);

	//int index for selection from available tiles
	int loc = TileManagerRef->GameStream.RandRange(0, selected.neighborArray.Num() - 1);

	//we now have our room we selected and the neighbor in which we are using for our secret room
	FVector SpawnPos;
	FRotator SpawnRot = FRotator(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//UE_LOG(LogTemp, Log, TEXT("Picked - %d - side of [%d,%d]"), selected.neighborArray[loc], selected.tile->XIndex, selected.tile->ZIndex);
	//UE_LOG(LogTemp, Log, TEXT("Selected:  x= %d, y = %d], z = %d"), test->GetActorLocation().X, test->GetActorLocation().Y, test->GetActorLocation().Z);

	FString name = test->GetActorLabel();
	//ASTile* tempTile;
	//UE_LOG(LogTemp, Log, TEXT("Tile: %s"), *name);
	FVector Origin;
	FVector Extents;
	FString TileDoorName;

	//TODO: weird but with center of tile being at the top, causing a 240 offset. Will need to investigate later
	switch (selected.neighborArray[loc])
	{
	case 1:
		//TODO: may need to fix rotation? 
		if (selected.tile->UpNeighbor == NULL)
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X, selected.tile->GetActorLocation().Y - (TileManagerRef->StartingTile->TileLength), selected.tile->GetActorLocation().Z); //+ 240
			//UE_LOG(LogTemp, Log, TEXT("SpawnPas: %s"), *SpawnPos.ToString());
			//SpawnRot = FRotator(selected.tile->GetActorRotation().Euler().X, 180.0f, selected.tile->GetActorRotation().Euler().Z);
			TileManagerRef->SecretRoom = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, SpawnRot, SpawnParams);
			SpawnDoor(TileManagerRef->SecretRoom, ETileSide::ETile_Down, "SecretRoom");
		}
		else if (selected.tile->UpNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) { //confirmed this works now get other wey of working
			//rotate tile? may need tile to be setup for easier testing of rotation
			TileManagerRef->SecretRoom = selected.tile->UpNeighbor;
			SetupDoor(TileManagerRef->SecretRoom, ETileSide::ETile_Down, "SecretRoom", selected.tile->UpDoor);
		}

		TileManagerRef->SecretRoom->DownNeighbor = selected.tile;
		selected.tile->UpDoor = TileManagerRef->SecretRoom->DownDoor;
		selected.tile->UpNeighbor = TileManagerRef->SecretRoom;

		break;
	case 2:
		//down

		//TODO: may need to fix rotation? 
		if (selected.tile->DownNeighbor == NULL)
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X, selected.tile->GetActorLocation().Y + (TileManagerRef->StartingTile->TileLength), selected.tile->GetActorLocation().Z);
			//UE_LOG(LogTemp, Log, TEXT("SpawnPas: %s"), *SpawnPos.ToString());
			SpawnRot = FRotator(selected.tile->GetActorRotation().Euler().X, 180.0f, selected.tile->GetActorRotation().Euler().Z);
			TileManagerRef->SecretRoom = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, SpawnRot, SpawnParams);
			SpawnDoor(TileManagerRef->SecretRoom, ETileSide::ETile_Up, "SecretRoom");
		}
		else if (selected.tile->DownNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) { //confirmed this works now get other wey of working
			//rotate tile? may need tile to be setup for easier testing of rotation
			TileManagerRef->SecretRoom = selected.tile->DownNeighbor;
			SetupDoor(TileManagerRef->SecretRoom, ETileSide::ETile_Up, "SecretRoom", selected.tile->DownDoor);
		}

		TileManagerRef->SecretRoom->UpNeighbor = selected.tile;
		selected.tile->DownDoor = TileManagerRef->SecretRoom->UpDoor;
		selected.tile->DownNeighbor = TileManagerRef->SecretRoom;

		break;
	case 3:
		//right

		//TODO: may need to fix rotation? 
		if (selected.tile->LeftNeighbor == NULL)
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X - (selected.tile->TileLength), selected.tile->GetActorLocation().Y, selected.tile->GetActorLocation().Z); //+ 240;
			//UE_LOG(LogTemp, Log, TEXT("SpawnPas: %s"), *SpawnPos.ToString());
			SpawnRot = FRotator(TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().X, 90.0f, TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().Z);
			TileManagerRef->SecretRoom = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, SpawnRot, SpawnParams);
			SpawnDoor(TileManagerRef->SecretRoom, ETileSide::ETile_Right, "SecretRoom");
		}
		else if (selected.tile->LeftNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) { //confirmed this works now get other wey of working
			//rotate tile? may need tile to be setup for easier testing of rotation
			TileManagerRef->SecretRoom = selected.tile->LeftNeighbor;
			SetupDoor(TileManagerRef->SecretRoom, ETileSide::ETile_Right, "SecretRoom", selected.tile->LeftDoor);
		}

		TileManagerRef->SecretRoom->RightNeighbor = selected.tile;
		selected.tile->LeftDoor = TileManagerRef->SecretRoom->RightDoor;
		selected.tile->LeftNeighbor = TileManagerRef->SecretRoom;

		break;
	case 4:
		//left

		//TODO: may need to fix rotation? 
		if (selected.tile->RightNeighbor == NULL)
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X + (selected.tile->TileLength), selected.tile->GetActorLocation().Y, selected.tile->GetActorLocation().Z + 240);
			//UE_LOG(LogTemp, Log, TEXT("SpawnPas: %s"), *SpawnPos.ToString());
			SpawnRot = FRotator(TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().X, -90.0f, TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().Z);
			TileManagerRef->SecretRoom = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, SpawnRot, SpawnParams);
			SpawnDoor(TileManagerRef->SecretRoom, ETileSide::ETile_Left, "SecretRoom");
		}
		else if (selected.tile->RightNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) { //confirmed this works now get other wey of working
			//rotate tile? may need tile to be setup for easier testing of rotation
			TileManagerRef->SecretRoom = selected.tile->RightNeighbor;
			SetupDoor(TileManagerRef->SecretRoom, ETileSide::ETile_Left, "SecretRoom", selected.tile->RightDoor);
		}

		TileManagerRef->SecretRoom->LeftNeighbor = selected.tile;
		selected.tile->RightDoor = TileManagerRef->SecretRoom->LeftDoor;
		selected.tile->RightNeighbor = TileManagerRef->SecretRoom;

		break;
	}
	// TO DO: this will need to be updated to a specific Secrete Room BP set in LocalLevel

	TileManagerRef->SecretRoom->SetActorLabel("SecretRoom");
	//SecretRoom->SetOwner(this);
#if WITH_EDITOR
	TileManagerRef->SecretRoom->SetFolderPath(TileManagerRef->TileSubFolderName);
	TileManagerRef->SecretRoom->ShadeSecretRoom();
#endif

	//ACTIVATE WALLS
	TileManagerRef->SecretRoom->ActivateWalls();
}

/// <summary>
/// Dylan Loe
/// 
/// - Remove all inactive doors
/// </summary>
void UTileGridBranchComponent::FinalDoorSetupDoors()
{
	for (int doorIndex = 0; doorIndex < TileManagerRef->DoorArray.Num(); doorIndex++)
	{
		//remove all doors inactive (aka the ones not connecting paths)
		ASTileDoor* door = TileManagerRef->DoorArray[doorIndex];
		if (!door->DoorActive)
		{
			//delete door
			door->Destroy();
		}
	}
}

/// <summary>
/// Dylan Loe
/// 
/// - Will run through once Secret room and Door configuration is implemented to remove NULL rooms from Tile Map
/// </summary>
void UTileGridBranchComponent::DeactiveInactiveRooms()
{
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Removing Unwanted tiles..."));

	for (FMultiTileStruct* row : TileManagerRef->Grid2DArray)
	{
		for (ASTile* tile : row->TileColumn)
		{
			if (tile->TileStatus == ETileStatus::ETile_NULLROOM)
			{
				//Destroy(tile);
				tile->Destroy();
			}
			//TODO: turn on walls at borders of path handled in levelassetspawn
		}

	}
}

/// <summary>
/// 
/// </summary>
/// <param name="TileToAdd"></param>
/// <param name="CurrentPath"></param>
/// <param name="Length"></param>
/// <param name="prevDirection"></param>
void UTileGridBranchComponent::CheckBranchTile(ASTile* TileToAdd, TArray<ASTile*>& CurrentPath, int Length, int prevDirection)
{
	//2,3 last one is 3,3
	//UE_LOG(LogTemp, Log, TEXT("Current Tile: %d,%d"), TileToAdd->XIndex, TileToAdd->ZIndex);
	if (Length > 0)
	{
		CurrentPath.Add(TileToAdd);
		TileToAdd->ShadeActiveRoom();
		TileToAdd->TileDescription = "";

		if ((!TileToAdd->HasValidRightNeighbor()) && (!TileToAdd->HasValidLeftNeighbor())
			&& (!TileToAdd->HasValidUpNeighbor()) && (!TileToAdd->HasValidDownNeighbor()))
		{
			//theres no where to go, lets just end the branch here to save time
			Length = 0;

			//TODO: Should there be a possibility of this end of branch connecting else where? or should it be purely linear?

			ConnectDoorBranch(TileToAdd, prevDirection);

			return;  //exit branch
		}

		TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };

		DirectionsToCheck = TileManagerRef->Reshuffle2(DirectionsToCheck);

		//pick direction and begin CheckTile
		for (int DirectionCount = 0; DirectionCount < DirectionsToCheck.Num(); DirectionCount++) {
			switch (DirectionsToCheck[DirectionCount]) {
			case 1:
				//UP
				if (TileToAdd->HasValidUpNeighbor() && !TileToAdd->UpNeighbor->CheckForPath && !TileToAdd->UpNeighbor->IsStartingTile()) {
					TileToAdd->UpNeighbor->PreviousTile = TileToAdd;
					Length--;
					ConnectDoorBranch(TileToAdd, prevDirection);
					CheckBranchTile(TileToAdd->UpNeighbor, CurrentPath, Length, 1);
					return;
				}
				break;
			case 2:
				//DOWN
				if (TileToAdd->HasValidDownNeighbor() && !TileToAdd->DownNeighbor->CheckForPath && !TileToAdd->DownNeighbor->IsStartingTile()) {
					TileToAdd->DownNeighbor->PreviousTile = TileToAdd;
					Length--;
					ConnectDoorBranch(TileToAdd, prevDirection);
					CheckBranchTile(TileToAdd->DownNeighbor, CurrentPath, Length, 2);
					return;
				}
				break;
			case 3:
				//LEFT
				if (TileToAdd->HasValidLeftNeighbor() && !TileToAdd->LeftNeighbor->CheckForPath && !TileToAdd->LeftNeighbor->IsStartingTile()) {
					TileToAdd->LeftNeighbor->PreviousTile = TileToAdd;
					Length--;
					ConnectDoorBranch(TileToAdd, prevDirection);
					CheckBranchTile(TileToAdd->LeftNeighbor, CurrentPath, Length, 3);
					return;
				}
				break;
			case 4:
				//RIGHT
				if (TileToAdd->HasValidRightNeighbor() && !TileToAdd->RightNeighbor->CheckForPath && !TileToAdd->RightNeighbor->IsStartingTile()) {
					TileToAdd->RightNeighbor->PreviousTile = TileToAdd;
					Length--;
					ConnectDoorBranch(TileToAdd, prevDirection);
					CheckBranchTile(TileToAdd->RightNeighbor, CurrentPath, Length, 4);
					return;
				}
				break;
			}
		}
	}

	return;
}

/// <summary>
/// pass in tile, check which side connects to path
/// </summary>
/// <param name="TileToAdd"></param>
/// <returns></returns>
int UTileGridBranchComponent::CheckPathSide(ASTile* TileToCheck)
{
	//needs to be random
	TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };

	DirectionsToCheck = TileManagerRef->Reshuffle2(DirectionsToCheck);

	int choice = 0;

	for (int DirectionCount = 0; DirectionCount < DirectionsToCheck.Num(); DirectionCount++) {
		switch (DirectionsToCheck[DirectionCount]) {

		case 2:
			//
			if (TileToCheck->HasConnectedUpNeighbor() && TileToCheck->UpNeighbor->CheckForPath && TileToCheck->UpNeighbor->IsNotSpecialTile())
			{
				return 2;
			}
			break;
		case 1:
			//
			if (TileToCheck->HasConnectedDownNeighbor() && TileToCheck->DownNeighbor->CheckForPath && TileToCheck->DownNeighbor->IsNotSpecialTile())
			{
				return 1;
			}
			break;
		case 4:
			//
			if (TileToCheck->HasConnectedLeftNeighbor() && TileToCheck->LeftNeighbor->CheckForPath && TileToCheck->LeftNeighbor->IsNotSpecialTile())
			{
				return 4;
			}
			break;
		case 3:
			//
			if (TileToCheck->HasConnectedRightNeighbor() && TileToCheck->RightNeighbor->CheckForPath && TileToCheck->RightNeighbor->IsNotSpecialTile())
			{
				return 3;
			}
			break;
		}
	}

	//if no direct path detected, check if room status
	DirectionsToCheck = TileManagerRef->Reshuffle2(DirectionsToCheck);
	for (int DirectionCount = 0; DirectionCount < DirectionsToCheck.Num(); DirectionCount++) {
		switch (DirectionsToCheck[DirectionCount]) {

		case 2:
			//
			if (TileToCheck->HasConnectedUpNeighbor() && TileToCheck->UpNeighbor->IsNotSpecialTile())
			{
				return 2;
			}
			break;
		case 1:
			//
			if (TileToCheck->HasConnectedDownNeighbor() && TileToCheck->DownNeighbor->IsNotSpecialTile())
			{
				return 1;
			}
			break;
		case 4:
			//
			if (TileToCheck->HasConnectedLeftNeighbor() && TileToCheck->LeftNeighbor->IsNotSpecialTile())
			{
				return 4;
			}
			break;
		case 3:
			//
			if (TileToCheck->HasConnectedRightNeighbor() && TileToCheck->RightNeighbor->IsNotSpecialTile())
			{
				return 3;
			}
			break;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("This should never run."));
	return -1;
}

/// <summary>
/// find a random side, and connect door to first room we find that isn't boss room, or starting room
/// </summary>
/// <param name="CurrentTile"></param>
void UTileGridBranchComponent::SingleRoomsDoorSetup(ASTile* CurrentTile)
{
	TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };
	DirectionsToCheck = TileManagerRef->Reshuffle2(DirectionsToCheck);

	//pick direction and begin CheckTile
	for (int DirectionCount = 0; DirectionCount < DirectionsToCheck.Num(); DirectionCount++)
	{
		UE_LOG(LogTemp, Log, TEXT("Number: %d"), DirectionsToCheck[DirectionCount]);
		switch (DirectionsToCheck[DirectionCount])
		{
		case 1:
			//check up side
			if (CurrentTile->HasConnectedUpNeighbor())
			{
				CurrentTile->ConnectUpDoor();
				return;
			}
			break;
			//
		case 2:
			if (CurrentTile->HasConnectedDownNeighbor())
			{
				CurrentTile->ConnectDownDoor();
				return;
			}
			break;
		case 3:
			if (CurrentTile->HasConnectedLeftNeighbor())
			{
				CurrentTile->ConnectLeftDoor();
				return;
			}
			break;
		case 4:
			if (CurrentTile->HasConnectedRightNeighbor())
			{
				CurrentTile->ConnectRightDoor();
				return;
			}
			break;
		}
	}
}

/// <summary>
/// For Spawning doors to attach to tiles
/// </summary>
/// <param name="tile"></param>
/// <param name="SideToSpawnDoor"></param>
/// <param name="NameOfTileToConnect"></param>
void UTileGridBranchComponent::SpawnDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FString TileDoorName = "TileDoorConnecting_" + FString::FromInt(tile->XIndex) + "_" + FString::FromInt(tile->ZIndex) + "_to_SecretRoom";

	FTransform doorSpawnPoint;
	switch (SideToSpawnDoor)
	{
	case ETileSide::ETile_Up:
		doorSpawnPoint = tile->UpDoorSpawnPoint;
		break;
	case ETileSide::ETile_Down:
		doorSpawnPoint = tile->DownDoorSpawnPoint;
		break;
	case ETileSide::ETile_Left:
		doorSpawnPoint = tile->LeftDoorSpawnPoint;
		break;
	case ETileSide::ETile_Right:
		doorSpawnPoint = tile->RightDoorSpawnPoint;
		break;
	default:
		break;
	}

	//this could be problematic line, the secret room's center is offset from normal tiles
	//before this was this objects transform not the tiles location
	const FVector doorSpawnLocation = doorSpawnPoint.GetLocation() + tile->GetActorLocation();
	//UE_LOG(LogTemp, Log, TEXT("THIS -> Actor Location: X=%f, Y=%f, Z=%f"), this->GetActorLocation().X, this->GetActorLocation().Y, this->GetActorLocation().Z);
	const FTransform Spawm = FTransform(doorSpawnPoint.GetRotation(), doorSpawnLocation);


	ASTileDoor* door = GetWorld()->SpawnActor<ASTileDoor>(TileManagerRef->TileDoor, Spawm, SpawnParams);

	SetupDoor(tile, SideToSpawnDoor, NameOfTileToConnect, door);
}

/// <summary>
/// 
/// TO DO: Doors should have some link to the corresponding wall, these walls must be special so the door can fit.
/// 
/// If doors are disabled, then remove those walls also?
/// 
/// </summary>
/// <param name="tile"></param>
/// <param name="SideToSpawnDoor"></param>
/// <param name="NameOfTileToConnect"></param>
/// <param name="door"></param>
void UTileGridBranchComponent::SetupDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect, ASTileDoor* door)
{
	const FString TileDoorName = "TileDoorConnecting_" + FString::FromInt(tile->XIndex) + "_" + FString::FromInt(tile->ZIndex) + "_to_SecretRoom";

	door->DoorActive = true;
	TileManagerRef->DoorArray.Add(door);
	door->SetActorLabel(TileDoorName);
	door->SetOwner(TileManagerRef);
#if WITH_EDITOR
	door->SetFolderPath(TileManagerRef->DoorSubFolderName);
#endif

	switch (SideToSpawnDoor)
	{
	case ETileSide::ETile_Up:
		tile->UpDoor = door;
		break;
	case ETileSide::ETile_Down:
		tile->DownDoor = door;
		break;
	case ETileSide::ETile_Left:
		tile->LeftDoor = door;
		break;
	case ETileSide::ETile_Right:
		tile->RightDoor = door;
		break;
	default:
		break;
	}
}

/// <summary>
/// When going through the branch tiles to add, we need to activate the door of the previous tile visited.
/// (Passed in via prevDirection where 1 = Up, 2 = down, 3 = left, 4 = right
/// </summary>
/// <param name="TileToAdd"></param>
/// <param name="prevDirection"></param>
void UTileGridBranchComponent::ConnectDoorBranch(ASTile* TileToAdd, int prevDirection)
{
	switch (prevDirection) {
	case 1: //prev was up
		//prev tile was up direction to get here, therefore this tile's down neighbor was the up neighbor of the prev tile
		TileToAdd->ConnectDownDoor();
		break;
	case 2: //prev was down
		TileToAdd->ConnectUpDoor();
		break;
	case 3: //prev was left
		TileToAdd->ConnectRightDoor();
		break;
	case 4: //prev was right
		TileToAdd->ConnectLeftDoor();
		break;
	}
}