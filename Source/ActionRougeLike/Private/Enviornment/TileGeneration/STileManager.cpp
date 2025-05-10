// Fill out your copyright notice in the Description page of Project Settings.


#include "Enviornment/TileGeneration/STileManager.h"
#include "SLocalLevel.h"
#include <string>
#include <Math/UnrealMathUtility.h>
#include <Kismet/KismetMathLibrary.h>

// Sets default values
ASTileManager::ASTileManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TilesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TilesRoot"));
	RootComponent = TilesRoot;

}



void ASTileManager::SeedSetup()
{
	if (GameSeed == 0)
	{
		if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Setting Up Seed..."));

		GameStream.Initialize("GameSeed");
		GameStream.GenerateNewSeed();
	}
	else {
		if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Using Supplied Seed..."));
		//GameStream.Initialize("GameSeed");
		GameStream.Initialize(GameSeed);

	}

	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Seed: %d"), GameStream.GetCurrentSeed());
}

/// <summary>
/// Dylan Loe
/// 
/// - on begin play, 2d array is populated and established. For now we will use hard coded sizes. 
/// TO DO: Switch to Scriptable objects to determine difficulty and sizing
/// </summary>
void ASTileManager::BeginPlay()
{
	Super::BeginPlay();
	if (DebugPrints) {
		UE_LOG(LogTemp, Log, TEXT("==========================================================="));
		UE_LOG(LogTemp, Log, TEXT("================= TILE GENERATION ========================="));
		UE_LOG(LogTemp, Log, TEXT("==========================================================="));
	}
	SeedSetup();
	TileGeneration();
}

/// <summary>
/// Dylan Loe
/// 
/// - Create Tile map structure and run procedure to create gameplay path and populate the map
/// </summary>
void ASTileManager::TileGeneration()
{
	//create and link tiles into grid
	//this includes establishment of doors if we need them
	Create2DTileArray();

	//once tiles are established, we now pick the starting tile
	ChooseStartEndRooms();

}

/// <summary>
/// Dylan Loe
/// 
/// Creation of 2d array for tiles
/// Creates tiles
/// </summary>
void ASTileManager::Create2DTileArray()
{

	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Creating 2D array! =============================="));

	for (int32 XIndex = 0; XIndex < LevelWidth; XIndex++)
	{
		//for each row, make each column
		FMultiTileStruct* Col = new FMultiTileStruct();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		int tileLength = TileBase->GetDefaultObject<ASTile>()->TileLength;
		//Populate TileColumn array with Tiles for Height size
		for (int32 ZIndex = 0; ZIndex < LevelHeight; ZIndex++)
		{
			//spawn in a Tile
			FString TileName = "Tile_Row" + FString::FromInt(XIndex) + "_Col" + FString::FromInt(ZIndex);
			ASTile* T = GetWorld()->SpawnActor<ASTile>(TileBase, FVector((this->GetActorLocation().X + (tileLength * XIndex)), (this->GetActorLocation().Y + (tileLength * ZIndex)), this->GetActorLocation().Z), this->GetActorRotation(), SpawnParams);
			T->SetActorLabel(TileName);
			T->SetOwner(this);
#if WITH_EDITOR
			T->SetFolderPath(TileSubFolderName);
#endif

			T->XIndex = XIndex;
			T->ZIndex = ZIndex;

			//add tile to array
			Col->TileColumn.Add(T);
			LinkTile(T, *Col);
			T->ShadeNull();
		}

		Grid2DArray.Add(Col);
	}
	totalGridTilesAvailable = (LevelHeight * LevelWidth) * gridDensity;
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== 2D array CREATED! =============================="));
}

/// <summary>
/// Dylan Loe
/// 
/// Choosing the start and end room
/// 
/// TO DO: Do i really need to pick the end room?
/// 
/// </summary>
void ASTileManager::ChooseStartEndRooms()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Choosing start and end rooms =============================="));

	int startX = 0, startY = 0;
	//will pick a random side and random tile on side to start
	if (StartRoomSide == -1)
		StartRoomSide = GameStream.RandRange(0, 3);
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Side Picked: %d"), StartRoomSide);



	switch (StartRoomSide) {
	case 0:
		//starting
		startY = 0;
		startX = GameStream.RandRange(0, LevelHeight - 1);
		//UE_LOG(LogTemp, Log, TEXT("num Picked: %d"), startX);
		//DOWN;

		//ending possible 
		//opposite of startX (0) would be LevelWidth - 1
		//for choosing the end tile, we will pick the opposite 2 rows/columns
		//must be a distance of height/2 and/or column/2 to be added to a list to be randomly picked as end room
		//cant pick tiles that are on the same row or column (no straight shots)
		for (int index2 = 0; index2 < (LevelWidth - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startX; index++)
			{
				//ASTile* Possible = Grid2DArray[LevelWidth - 1]->TileColumn[index];
				PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]);
				Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = LevelHeight - 1; index > startX; index--)
			{
				PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]);
				Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]->ShadeTestRoom();
			}
		}

		//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
		if (startX > (LevelWidth - 1) / 2) { //more than half AFTER startX
			for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
				for (int index3 = 0; index3 < LevelHeight - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index3, extra);
					PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]);
					Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]->ShadeTestRoom();
				}
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
		if (startX < ((LevelWidth - 1) / 2)) { //more than after BEFORE startX
			for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
				for (int index4 = 0; index4 < LevelHeight - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index4, LevelWidth - 1 - extra);
					PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1 - extra]);
					Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1 - extra]->ShadeTestRoom();
				}
			}
		}


		break;
	case 1:
		startY = GameStream.RandRange(0, LevelWidth - 1);
		startX = 0;
		//LEFT;

		for (int index2 = 0; index2 < (LevelHeight - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startY; index++)
			{
				//ASTile* Possible = Grid2DArray[LevelWidth - 1]->TileColumn[index];
				PossibleStartingTiles.Add(Grid2DArray[index]->TileColumn[LevelHeight - 1 - index2]);
				Grid2DArray[index]->TileColumn[LevelHeight - 1 - index2]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = LevelHeight - 1; index > startY; index--)
			{
				PossibleStartingTiles.Add(Grid2DArray[index]->TileColumn[LevelHeight - 1 - index2]);
				Grid2DArray[index]->TileColumn[LevelHeight - 1 - index2]->ShadeTestRoom();
			}
		}


		//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d < %d"), startY, (LevelHeight - 1) / 2);
		if (startY > (LevelHeight - 1) / 2) { //more than half BEFORE startX
			//UE_LOG(LogTemp, Log, TEXT("Check: %d"), (startY)-((LevelWidth) / 2));
			for (int extra = 0; extra < (startY)-((LevelWidth) / 2); extra++) {
				for (int index3 = 0; index3 < LevelWidth - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), extra, LevelWidth - 1 - index3);
					PossibleStartingTiles.Add(Grid2DArray[extra]->TileColumn[LevelWidth - 1 - index3]);
					Grid2DArray[extra]->TileColumn[LevelWidth - 1 - index3]->ShadeTestRoom();
				}
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d > %d"), startY, (LevelHeight - 1) / 2);
		if (startY < ((LevelHeight - 1) / 2)) { //more than after AFTER startX
			//UE_LOG(LogTemp, Log, TEXT("Check: %d"), (LevelWidth - startY - 1) - ((LevelWidth) / 2));
			for (int extra = 0; extra < (LevelWidth - startY - 1) - ((LevelWidth) / 2); extra++) {
				for (int index4 = 0; index4 < LevelWidth - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - extra, LevelWidth - 1 - index4);
					PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - extra]->TileColumn[LevelWidth - 1 - index4]);
					Grid2DArray[LevelHeight - 1 - extra]->TileColumn[LevelWidth - 1 - index4]->ShadeTestRoom();
				}
			}
		}


		break;
	case 2:
		startY = LevelWidth - 1;
		startX = GameStream.RandRange(0, LevelHeight - 1);
		//UP;

		for (int index2 = 0; index2 < (LevelWidth - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startX; index++)
			{
				PossibleStartingTiles.Add(Grid2DArray[0 + index2]->TileColumn[index]);
				Grid2DArray[0 + index2]->TileColumn[index]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = LevelHeight - 1; index > startX; index--)
			{
				PossibleStartingTiles.Add(Grid2DArray[0 + index2]->TileColumn[index]);
				Grid2DArray[0 + index2]->TileColumn[index]->ShadeTestRoom();
			}
		}

		//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
		if (startX > (LevelWidth - 1) / 2) { //more than half AFTER startX
			for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
				for (int index3 = 0; index3 < LevelHeight - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), index3, extra);
					PossibleStartingTiles.Add(Grid2DArray[index3]->TileColumn[extra]);
					Grid2DArray[index3]->TileColumn[extra]->ShadeTestRoom();
				}
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
		if (startX < ((LevelWidth - 1) / 2)) { //more than after BEFORE startX
			for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
				for (int index4 = 0; index4 < LevelHeight - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), index4, LevelWidth - 1 - extra);
					PossibleStartingTiles.Add(Grid2DArray[index4]->TileColumn[LevelWidth - 1 - extra]);
					Grid2DArray[index4]->TileColumn[LevelWidth - 1 - extra]->ShadeTestRoom();
				}
			}
		}

		break;
	case 3:
		startY = GameStream.RandRange(0, LevelHeight - 1);
		startX = LevelWidth - 1;
		//RIGHT;

		for (int index2 = 0; index2 < (LevelHeight - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startY; index++)
			{
				PossibleStartingTiles.Add(Grid2DArray[index]->TileColumn[index2]);
				Grid2DArray[index]->TileColumn[index2]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = LevelWidth - 1; index > startY; index--)
			{
				PossibleStartingTiles.Add(Grid2DArray[index]->TileColumn[index2]);
				Grid2DArray[index]->TileColumn[index2]->ShadeTestRoom();
			}
		}
		if (startY > (LevelHeight - 1) / 2) { //more than half BEFORE startX
			for (int extra = 0; extra < (startY)-((LevelWidth) / 2); extra++) {
				//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startY, (LevelHeight - 1) / 2);

				for (int index3 = 0; index3 < LevelHeight - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), extra, index3);
					PossibleStartingTiles.Add(Grid2DArray[extra]->TileColumn[index3]);
					Grid2DArray[extra]->TileColumn[index3]->ShadeTestRoom();
				}
			}
		}
		if (startY < ((LevelHeight - 1) / 2)) { //more than after AFTER startX
			for (int extra = 0; extra < (LevelWidth - startY - 1) - ((LevelWidth) / 2); extra++) {
				//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startY, (LevelHeight - 1) / 2);
				for (int index4 = 0; index4 < LevelHeight - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"),  LevelWidth - 1 - extra, index4);
					PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - extra]->TileColumn[index4]);
					Grid2DArray[LevelWidth - 1 - extra]->TileColumn[index4]->ShadeTestRoom();
				}
			}
		}

		break;
	default:
		if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Issue here -> picked weird starting side in ChooseStartEndRooms"));
		//startY = 0;
		//startX = GameStream.RandRange(0, LevelHeight - 1);
		//DOWN	
		for (int index2 = 0; index2 < (LevelWidth - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startX; index++)
			{
				//ASTile* Possible = Grid2DArray[LevelWidth - 1]->TileColumn[index];
				PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]);
				Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = LevelHeight - 1; index > startX; index--)
			{
				PossibleStartingTiles.Add(Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]);
				Grid2DArray[LevelWidth - 1 - index2]->TileColumn[index]->ShadeTestRoom();
			}
		}

		//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
		if (startX > (LevelWidth - 1) / 2) { //more than half AFTER startX
			for (int extra = 0; extra < (startX)-((LevelHeight) / 2); extra++) {
				for (int index3 = 0; index3 < LevelHeight - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index3, extra);
					PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]);
					Grid2DArray[LevelHeight - 1 - index3]->TileColumn[extra]->ShadeTestRoom();
				}
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
		if (startX < ((LevelWidth - 1) / 2)) { //more than after BEFORE startX
			for (int extra = 0; extra < (LevelHeight - startX - 1) - ((LevelHeight) / 2); extra++) {
				for (int index4 = 0; index4 < LevelHeight - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index4, LevelWidth - 1 - extra);
					PossibleStartingTiles.Add(Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1 - extra]);
					Grid2DArray[LevelHeight - 1 - index4]->TileColumn[LevelWidth - 1 - extra]->ShadeTestRoom();
				}
			}
		}

		break;
	}
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Starting Tile is designated as [%d,%d]"), startY, startX);

	StartingTile = Grid2DArray[startY]->TileColumn[startX];
	StartingTile->ShadeStartingRoom();

	//end room is picked randomly from array of possible rooms
	EndTile = PossibleStartingTiles[GameStream.RandRange(0, PossibleStartingTiles.Num() - 1)];
	EndTile->ShadeEndRoom();

	GeneratePath();
}

//Use recursive backtracking maze algorithm
void ASTileManager::GeneratePath()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Genearating Path =============================="));

	//add starting room to be start of list
	//AddTileToPath(StartingTile);

	CheckTile(StartingTile, LevelPath);

	if (DebugPrints) {
		//draw lines through path
		for (int Index = 0; Index < LevelPath.Num() - 1; Index++)
		{
			DrawDebugLine(GetWorld(), LevelPath[Index]->GetActorLocation(), LevelPath[Index + 1]->GetActorLocation(), FColor::Blue, SDPG_World, 20.0f, 150);
			//GetWorld()->LineBatcher->DrawLine(LevelPath[Index]->GetActorLocation(), LevelPath[Index + 1]->GetActorLocation(), FColor::Blue, SDPG_World, 10.0f, 100);
		}
	}

	CreateSpawnRoom();

	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished Path - Adding Random Rooms =============================="));

	RandomRoomsAndBranchesAdditions();

	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished Random Rooms - Adding Spawn Room =============================="));



	AddSingleRooms();

	if (MyLocalLevel->CurrentLevelTier >= ELevelTier::ELevel_2) {
		if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("=================== Finished Spawn Room - Adding Secret Room =============================="));
		CreateSecretRoom();

	}


	if (DoorsActive) {
		if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("=================== Finished Secret Room - Activating All Doors =============================="));

		FinalDoorSetupDoors();
	}

	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Finished All Doors - Implementing Final Tile Setup =============================="));

	DeactiveInactiveRooms();

	//if (DebugPrints)
	//	UE_LOG(LogTemp, Log, TEXT("=================== Finished Implementing Final Tile Setup - Beginning Populating Grid =============================="));
}

bool ASTileManager::AddTileToPath(ASTile* TileToAdd)
{
	PathNumber++;
	LevelPath.AddUnique(TileToAdd);
	TileToAdd->CheckForPath = true;
	TileToAdd->PathNumber = PathNumber;
	if (!TileToAdd->IsBossTile() && !TileToAdd->IsStartingTile())
		TileToAdd->ShadePath();

	return true;
}

/// <summary>
/// Backtracking reclusive algorithm for Main level path construction. Builds out LevelPath array.
/// TODO: As we go, doors connecting tiles will be marked, after everything is done, unmarked doors get destroyed at the end (no need to have second pass checking)
/// </summary>
/// <param name="CurrentTile"></param>
/// <param name="CurrentPath"></param>
void ASTileManager::CheckTile(ASTile* CurrentTile, TArray<ASTile*>& CurrentPath)
{
	bool CheckingTileDebug = false;


	// For Debug Check, for now will be off since this check is no longer critical
	if (CurrentTile) {
		if (CheckingTileDebug)
		{
			UE_LOG(LogTemp, Log, TEXT("Currently on Tile: %d,%d"), CurrentTile->XIndex, CurrentTile->ZIndex);
		}
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("NULL TILE DETECTED. PLEASE INVESTIGATE"));
	}

	if (FailsafeCount == LevelHeight * LevelWidth * 2)
	{
		UE_LOG(LogTemp, Log, TEXT("NHitting dead ends. PLEASE INVESTIGATE"));
	}
	else {
		FailsafeCount++;
	}

	//first check if all neighbors are unavailable
	//if so, make this one as checked and call on previous tiles
	if ((!CurrentTile->UpNeighbor || CurrentTile->UpNeighbor->CheckForPath) && (!CurrentTile->DownNeighbor || CurrentTile->DownNeighbor->CheckForPath)
		&& (!CurrentTile->RightNeighbor || CurrentTile->RightNeighbor->CheckForPath) && (!CurrentTile->LeftNeighbor || CurrentTile->LeftNeighbor->CheckForPath))
	{
		UE_LOG(LogTemp, Log, TEXT("Dead end found at %d,%d"), CurrentTile->XIndex, CurrentTile->ZIndex);
		CurrentTile->CheckForPath = true;
		CurrentTile->ShadeNull();

		//eventually all of these tiles will be unchecked so a different path may go through them (dead ends,etc)
		BackTrackHistory.Add(CurrentTile);
		CurrentPath.Remove(CurrentTile);

		CurrentTile->TurnAllDoorsInactive();

		//E_LOG(LogTemp, Log, TEXT("Removed Instances: %d"), CurrentPath.Remove(CurrentTile).toString());
		CheckTile(CurrentTile->PreviousTile, CurrentPath);
	} // boss room checks
	else if ((CurrentTile->UpNeighbor && CurrentTile->UpNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->UpNeighbor->XIndex, CurrentTile->UpNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->UpNeighbor);

		CurrentTile->ConnectUpDoor();
		doorTransform = CurrentTile->UpDoor->GetTransform();
	}
	else if ((CurrentTile->DownNeighbor && CurrentTile->DownNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->DownNeighbor->XIndex, CurrentTile->DownNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->DownNeighbor);
		CurrentTile->ConnectDownDoor();
		doorTransform = CurrentTile->DownDoor->GetTransform();
	}
	else if ((CurrentTile->RightNeighbor && CurrentTile->RightNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->RightNeighbor->XIndex, CurrentTile->RightNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->RightNeighbor);
		CurrentTile->ConnectRightDoor();
		doorTransform = CurrentTile->RightDoor->GetTransform();
	}
	else if ((CurrentTile->LeftNeighbor && CurrentTile->LeftNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->LeftNeighbor->XIndex, CurrentTile->LeftNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->LeftNeighbor);
		CurrentTile->ConnectLeftDoor();
		doorTransform = CurrentTile->LeftDoor->GetTransform();
	}
	else {
		//now that we know theres valid neighbors and none of them are the boss room, lets check our neighbors

		//UE_LOG(LogTemp, Log, TEXT("Path Checking: %d,%d"), CurrentTile->XIndex, CurrentTile->ZIndex);
		//direction
		TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };

		DirectionsToCheck = Reshuffle2(DirectionsToCheck);

		//pick direction and begin CheckTile
		for (int DirectionCount = 0; DirectionCount < DirectionsToCheck.Num(); DirectionCount++)
		{
			switch (DirectionsToCheck[DirectionCount])
			{
			case 1:
				//UP
				if (CurrentTile->HasValidUpNeighbor() && !CurrentTile->UpNeighbor->CheckForPath && !CurrentTile->UpNeighbor->IsStartingTile())
				{
					//add this tile to path, go to up neighbor
					CurrentTile->ActivateUpDoor();
					CurrentTile->UpNeighbor->PreviousTile = CurrentTile;
					AddTileToPath(CurrentTile);
					//no need to keep going through other directions directions
					DirectionCount = 5;
					doorTransform = CurrentTile->UpDoor->GetTransform();

					CheckTile(CurrentTile->UpNeighbor, CurrentPath);
				}
				break;
			case 2:
				//DOWN
				if (CurrentTile->HasValidDownNeighbor() && !CurrentTile->DownNeighbor->CheckForPath && !CurrentTile->DownNeighbor->IsStartingTile())
				{
					CurrentTile->ActivateDownDoor();
					CurrentTile->DownNeighbor->PreviousTile = CurrentTile;
					AddTileToPath(CurrentTile);
					DirectionCount = 5;
					doorTransform = CurrentTile->DownDoor->GetTransform();

					CheckTile(CurrentTile->DownNeighbor, CurrentPath);
				}
				break;
			case 3:
				//LEFT
				if (CurrentTile->HasValidLeftNeighbor() && !CurrentTile->LeftNeighbor->CheckForPath && !CurrentTile->LeftNeighbor->IsStartingTile())
				{
					CurrentTile->ActivateLeftDoor();
					CurrentTile->LeftNeighbor->PreviousTile = CurrentTile;
					AddTileToPath(CurrentTile);
					DirectionCount = 5;
					doorTransform = CurrentTile->LeftDoor->GetTransform();
					CheckTile(CurrentTile->LeftNeighbor, CurrentPath);
				}
				break;
			case 4:
				//RIGHT
				if (CurrentTile->HasValidRightNeighbor() && !CurrentTile->RightNeighbor->CheckForPath && !CurrentTile->RightNeighbor->IsStartingTile())
				{
					CurrentTile->ActivateRightDoor();
					CurrentTile->RightNeighbor->PreviousTile = CurrentTile;
					AddTileToPath(CurrentTile);
					DirectionCount = 5;
					doorTransform = CurrentTile->RightDoor->GetTransform();
					CheckTile(CurrentTile->RightNeighbor, CurrentPath);
				}
				break;
			}
		}
	}
}


/// <summary>
/// Dylan Log
/// History is mostly for debug, removes previous backtrack history so we can reuse same variable
/// </summary>
void ASTileManager::ClearHistory()
{
	for (int BIndex = 0; BIndex < BackTrackHistory.Num(); BIndex++)
	{
		BackTrackHistory[BIndex]->CheckForPath = false;
	}
}

/// <summary>
/// Dylan Loe
/// 
/// - Adding Single random and branches to grid
/// </summary>
void ASTileManager::RandomRoomsAndBranchesAdditions()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Adding Branches"));

	AllActiveTiles.Append(LevelPath);

	MakeAvailableTiles();

	//TODO: how long will branches be? Get better way to find this
	//Ideas/Research: 
	// - Adaptive Branch grown: iteratively grow branches based on available space, after/during each branch, check if we can still make more to a certain amount
	// - Controlled Density approach: define the number of branches based on total maze size and expected complexity
	//Controlled Density approach
	int TotalBranchesMax1 = FMath::RoundToInt(LevelWidth * LevelHeight * BranchDensityFactor_DynamicMainPathLength());
	//Dependency On maze size approach
	int TotalBranchesMax2 = FMath::RoundToInt(LevelWidth * LevelHeight * BranchDensityFactor_DependencyOnMazeSize());
	// - Directional Bias Control: weighted probability function, where branch creation probability decreases as the main path progresses

	int oldWay = (LevelWidth - LevelPath.Num() / LevelWidth) + 1;
	//for some randomness
	int TotalBranchesMax = GameStream.RandRange(1, TotalBranchesMax1);

	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Total amount of branches to create: %d"), TotalBranchesMax);

	for (int CurrentBranch = 0; CurrentBranch < TotalBranchesMax && AvailableTiles.Num() > 1; CurrentBranch++)
	{
		//for now using length of level, might change this later, not sure how else but not a super important detail
		int BranchLength = GameStream.RandRange(1, LevelWidth);
		if (DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Making Branch: %d with length %d"), CurrentBranch, BranchLength);

		//pick random index that isn't boss tile
		//pick a valid neighbor that isn't part of path or outside grid, thats not a boss tile
		//this tile is now the start of a branch

		//starting tile for branch
		int indexChoosen = GameStream.RandRange(0, AvailableTiles.Num() - 1);
		ASTile* StartingBranchTile = AvailableTiles[indexChoosen];

		TArray<ASTile*>	BranchArray;

		//remove starting branch tile
		AvailableTiles.Remove(StartingBranchTile);

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

			if (!AllActiveTiles.Contains(BranchArray[BranchIndex]))
			{
				AllActiveTiles.AddUnique(BranchArray[BranchIndex]);
			}
			else if (AvailableTiles.Contains(BranchArray[BranchIndex]))
			{
				AvailableTiles.Remove(BranchArray[BranchIndex]);
			}

			if (BranchIndex == BranchArray.Num() - 1)
			{
				BranchArray[BranchIndex]->TileDescription += "ENDBRANCH";
				BranchArray[BranchIndex]->EndOfBranchPath = true;
			}
		}

		//once we make branch, we go back through and remake the available tile spots
		MakeAvailableTiles();

		//Debug draw branch
		if (DebugPrints) {

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
void ASTileManager::AddSingleRooms()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Adding Single Rooms..."));
	//when we add a room, remove it from AvailableTiles, add to AllActiveTiles

	//default to half the rooms left over
	FillerRooms = GameStream.RandRange(1, (LevelWidth - AvailableTiles.Num() - 1) - ((LevelHeight - AvailableTiles.Num() - 1) / 4));
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Total Random Single Rooms: %d"), FillerRooms);

	for (int STileCount = 0; STileCount < FillerRooms; STileCount++)
	{
		//total tiles * density percentage
		//ensure that total tiles / used tiles doesn't exceed this percentage

		//if (LevelWidth - AllActiveTiles.Num() >= LevelHeight / (LevelWidth * 2))
		if (gridDensity >= GetCurrentGridDensity())
		{
			ASTile* Current = AvailableTiles[GameStream.RandRange(0, AvailableTiles.Num() - 1)];
			//UE_LOG(LogTemp, Log, TEXT("Room selected: %d:%d"), Current->XIndex, Current->ZIndex);
			if (Current->TileStatus == ETileStatus::ETile_NULLROOM)
			{
				Current->ShadeActiveRoom();
				AvailableTiles.Remove(Current);
				AllActiveTiles.AddUnique(Current);
				Current->TileDescription = "Random Single Room";

				SingleRoomsDoorSetup(Current);

				//Activate Doors
				//Remake new available list (with this currents neighbors now added
				//TO DO - Size small: Optimize the remake so that we only add the new tiles rooms instead of having to go through entire list

				if (DoorsActive)
				{
					/*Current->ActivateDoorsRandom();*/
				}
				MakeAvailableTiles();
			}
		}
		else
		{
			//stop adding random tiles
			return;
		}
	}


}

//find a random side, and connect door to first room we find that isn't boss room, or starting room
void ASTileManager::SingleRoomsDoorSetup(ASTile* CurrentTile)
{
	TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };
	DirectionsToCheck = Reshuffle2(DirectionsToCheck);

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
/// Dylan Loe
/// 
/// - Add spawn room, connected to  the start room (this will be outside of the grid
/// </summary>
void ASTileManager::CreateSpawnRoom()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Creating Spawn Room..."));

	FVector SpawnPos;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//There are going to be 2 tiles basically spawned, one is the base, the base structure of the tile
	// The other tile (which i don't think needs to be a tile at all), is the environment to be populated on the tile
	// Since the environment will be rotated, its neighbors will be changed theretofore for now it will be faster to have its own base that stays static)
	switch (StartRoomSide)
	{
	case 0:
		//RIGHT
		SpawnPos = FVector(StartingTile->GetActorLocation().X - (StartingTile->TileLength), StartingTile->GetActorLocation().Y, StartingTile->GetActorLocation().Z);
		PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams); //rotate -90
		StartingTile->LeftNeighbor = PlayerStartingTileBase;
		PlayerStartingTileBase->RightNeighbor = StartingTile;
		PlayerSpawnPresentTile->SetActorRotation(FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, -90.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z));
		UE_LOG(LogTemp, Log, TEXT("No door spawned in start room, add code if we use this route..."));

		break;
	case 1:
		//UP
		SpawnPos = FVector(StartingTile->GetActorLocation().X, StartingTile->GetActorLocation().Y - (StartingTile->TileLength), StartingTile->GetActorLocation().Z);
		PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		StartingTile->UpNeighbor = PlayerStartingTileBase;
		PlayerStartingTileBase->DownNeighbor = StartingTile;
		UE_LOG(LogTemp, Log, TEXT("No door spawned in start room, add code if we use this route..."));



		break;
	case 2:
		//LEFT
		SpawnPos = FVector(StartingTile->GetActorLocation().X + (StartingTile->TileLength), StartingTile->GetActorLocation().Y, StartingTile->GetActorLocation().Z);
		PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams); //rotate 90
		StartingTile->RightNeighbor = PlayerStartingTileBase;
		PlayerStartingTileBase->LeftNeighbor = StartingTile;
		PlayerSpawnPresentTile->SetActorRotation(FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, 90, PlayerSpawnPresentTile->GetActorRotation().Euler().Z));
		UE_LOG(LogTemp, Log, TEXT("No door spawned in start room, add code if we use this route..."));


		break;
	case 3:
		//DOWN
		SpawnPos = FVector(StartingTile->GetActorLocation().X, StartingTile->GetActorLocation().Y + (StartingTile->TileLength), StartingTile->GetActorLocation().Z);
		PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams); //rotate 180
		PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevel->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		StartingTile->DownNeighbor = PlayerStartingTileBase;
		PlayerStartingTileBase->UpNeighbor = StartingTile;
		PlayerSpawnPresentTile->SetActorRotation(FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, 180.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z));

		if (DoorToStartRoom)
		{
			//Set up door - rn defaulted to 3 TODO: do i really need to even choose other sides to start on? is the player even gunna notice? perspective of player always starts the same
			//should always be having the same rotation right? therefore the door should always be up while everyone else's has different orientations
			const FString TileUpDoorName = "TileDoorConnecting_StartingRoom_to_" + FString::FromInt(PlayerStartingTileBase->UpNeighbor->XIndex) + "_" + FString::FromInt(PlayerStartingTileBase->UpNeighbor->ZIndex);
			const FVector UpDoorSpawnLocation = PlayerStartingTileBase->UpDoorSpawnPoint.GetLocation() + PlayerStartingTileBase->GetActorLocation();
			const FTransform Spawm = FTransform(PlayerStartingTileBase->UpDoorSpawnPoint.GetRotation(), UpDoorSpawnLocation);
			PlayerStartingTileBase->UpDoor = GetWorld()->SpawnActor<ASTileDoor>(TileDoor, Spawm, SpawnParams);
			DoorArray.Add(PlayerStartingTileBase->UpDoor);
			PlayerStartingTileBase->UpDoor->SetActorLabel(TileUpDoorName);
			PlayerStartingTileBase->UpDoor->SetOwner(StartingTile);
			
#if WITH_EDITOR
			PlayerStartingTileBase->UpDoor->SetFolderPath(DoorSubFolderName);
#endif
			StartingTile->DownDoor = PlayerStartingTileBase->UpDoor;
			//PlayerStartingTileBase->ActivateUpDoor();

			StartingTile->ConnectDownDoor();
		}
		break;
	}




	//label
	PlayerStartingTileBase->SetActorLabel("StartingTile_Base");
	PlayerSpawnPresentTile->SetActorLabel("StartingTile_Populate");
#if WITH_EDITOR
	PlayerStartingTileBase->SetFolderPath(TileGenRootFolder);
	PlayerSpawnPresentTile->SetFolderPath(TileGenRootFolder);
#endif
	//Set the Preset ref to the SpawnPresetTile obj
	PlayerStartingTileBase->PresetTile = PlayerSpawnPresentTile;

	//Spawn stats from StartingTile to PlayerStartingTileBase, then we will reassign the StartingTile
	StartingTile->ShadePath();
	LevelPath.Insert(PlayerStartingTileBase, 0);
	AllActiveTiles.Insert(PlayerStartingTileBase, 0);
	PlayerStartingTileBase->ShadeStartingRoom();
	StartingTile = PlayerStartingTileBase;
}

/// <summary>
/// Dylan Log
/// 
/// - Compose list of all null neighbors of all active tiles (excluding start and boss rooms)
/// - 
/// </summary>
void ASTileManager::CreateSecretRoom()
{
	TArray<ASTile*> outskirtsCheck;
	for (int tileCount = 0; tileCount < AllActiveTiles.Num(); tileCount++)
	{
		ASTile* currentTile = AllActiveTiles[tileCount];

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
				OutskirtTiles.Add(currentInfo);
				outskirtsCheck.Add(currentInfo.tile);
			}
		}
	}

	//now randomly pick a tile to put our secret room at (this tiles neighbor will be the secret room)
	int tileNum = GameStream.RandRange(0, OutskirtTiles.Num() - 1);

	FTileInfoStruct selected = OutskirtTiles[tileNum];
	ASTile* test = outskirtsCheck[tileNum];
	choosen = outskirtsCheck[tileNum];

	//reshuffle our n value
	selected.neighborArray = Reshuffle2(selected.neighborArray);

	//int index for selection from available tiles
	int loc = GameStream.RandRange(0, selected.neighborArray.Num() - 1);

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
			SpawnPos = FVector(selected.tile->GetActorLocation().X, selected.tile->GetActorLocation().Y - (StartingTile->TileLength), selected.tile->GetActorLocation().Z); //+ 240
			//UE_LOG(LogTemp, Log, TEXT("SpawnPas: %s"), *SpawnPos.ToString());
			//SpawnRot = FRotator(selected.tile->GetActorRotation().Euler().X, 180.0f, selected.tile->GetActorRotation().Euler().Z);
			SecretRoom = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, SpawnRot, SpawnParams);
			SpawnDoor(SecretRoom, ETileSide::ETile_Down, "SecretRoom", doorTransform);
		}
		else if (selected.tile->UpNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) { //confirmed this works now get other wey of working
			//rotate tile? may need tile to be setup for easier testing of rotation
			SecretRoom = selected.tile->UpNeighbor;
			SetupDoor(SecretRoom, ETileSide::ETile_Down, "SecretRoom", selected.tile->UpDoor);
		}

		SecretRoom->DownNeighbor = selected.tile;
		selected.tile->UpDoor = SecretRoom->DownDoor;
		selected.tile->UpNeighbor = SecretRoom;

		break;
	case 2:
		//down

		//TODO: may need to fix rotation? 
		if (selected.tile->DownNeighbor == NULL)
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X, selected.tile->GetActorLocation().Y + (StartingTile->TileLength), selected.tile->GetActorLocation().Z);
			//UE_LOG(LogTemp, Log, TEXT("SpawnPas: %s"), *SpawnPos.ToString());
			SpawnRot = FRotator(selected.tile->GetActorRotation().Euler().X, 180.0f, selected.tile->GetActorRotation().Euler().Z);
			SecretRoom = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, SpawnRot, SpawnParams);
			SpawnDoor(SecretRoom, ETileSide::ETile_Up, "SecretRoom", doorTransform);
		}
		else if (selected.tile->DownNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) { //confirmed this works now get other wey of working
			//rotate tile? may need tile to be setup for easier testing of rotation
			SecretRoom = selected.tile->DownNeighbor;
			SetupDoor(SecretRoom, ETileSide::ETile_Up, "SecretRoom", selected.tile->DownDoor);
		}

		SecretRoom->UpNeighbor = selected.tile;
		selected.tile->DownDoor = SecretRoom->UpDoor;
		selected.tile->DownNeighbor = SecretRoom;

		break;
	case 3:
		//right

		//TODO: may need to fix rotation? 
		if (selected.tile->LeftNeighbor == NULL)
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X - (selected.tile->TileLength), selected.tile->GetActorLocation().Y, selected.tile->GetActorLocation().Z); //+ 240;
			//UE_LOG(LogTemp, Log, TEXT("SpawnPas: %s"), *SpawnPos.ToString());
			SpawnRot = FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, 90.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z);
			SecretRoom = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, SpawnRot, SpawnParams);
			SpawnDoor(SecretRoom, ETileSide::ETile_Right, "SecretRoom", doorTransform);
		}
		else if (selected.tile->LeftNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) { //confirmed this works now get other wey of working
			//rotate tile? may need tile to be setup for easier testing of rotation
			SecretRoom = selected.tile->LeftNeighbor;
			SetupDoor(SecretRoom, ETileSide::ETile_Right, "SecretRoom", selected.tile->LeftDoor);
		}

		SecretRoom->RightNeighbor = selected.tile;
		selected.tile->LeftDoor = SecretRoom->RightDoor;
		selected.tile->LeftNeighbor = SecretRoom;

		break;
	case 4:
		//left

		//TODO: may need to fix rotation? 
		if (selected.tile->RightNeighbor == NULL)
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X + (selected.tile->TileLength), selected.tile->GetActorLocation().Y, selected.tile->GetActorLocation().Z + 240);
			//UE_LOG(LogTemp, Log, TEXT("SpawnPas: %s"), *SpawnPos.ToString());
			SpawnRot = FRotator(PlayerSpawnPresentTile->GetActorRotation().Euler().X, -90.0f, PlayerSpawnPresentTile->GetActorRotation().Euler().Z);
			SecretRoom = GetWorld()->SpawnActor<ASTile>(TileBase, SpawnPos, SpawnRot, SpawnParams);
			SpawnDoor(SecretRoom, ETileSide::ETile_Left, "SecretRoom", doorTransform);
		}
		else if (selected.tile->RightNeighbor->TileStatus == ETileStatus::ETile_NULLROOM) { //confirmed this works now get other wey of working
			//rotate tile? may need tile to be setup for easier testing of rotation
			SecretRoom = selected.tile->RightNeighbor;
			SetupDoor(SecretRoom, ETileSide::ETile_Left, "SecretRoom", selected.tile->RightDoor);
		}

		SecretRoom->LeftNeighbor = selected.tile;
		selected.tile->RightDoor = SecretRoom->LeftDoor;
		selected.tile->RightNeighbor = SecretRoom;

		break;
	}
	// TO DO: this will need to be updated to a specific Secrete Room BP set in LocalLevel

	SecretRoom->SetActorLabel("SecretRoom");
	//SecretRoom->SetOwner(this);
#if WITH_EDITOR
	SecretRoom->SetFolderPath(TileSubFolderName);
	SecretRoom->ShadeSecretRoom();
#endif

	//ACTIVATE WALLS
	SecretRoom->ActivateWalls();
}

/// <summary>
/// Dylan Loe
/// 
/// - Runs through branch recursively to make random branches out of main path
/// </summary>
/// <param name="TileToAdd"> Current Tile being analyzed </param>
/// <param name="CurrentPath"> Current branch path we are building </param>
/// <param name="Length"> given size of path we are aiming for </param>
void ASTileManager::CheckBranchTile(ASTile* TileToAdd, TArray<ASTile*>& CurrentPath, int Length, int prevDirection)
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

		DirectionsToCheck = Reshuffle2(DirectionsToCheck);

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
/// When going through the branch tiles to add, we need to activate the door of the previous tile visited.
/// (Passed in via prevDirection where 1 = Up, 2 = down, 3 = left, 4 = right
/// </summary>
/// <param name="TileToAdd"></param>
/// <param name="CurrentPath"></param>
/// <param name="Length"></param>
/// <param name="prevDirection"></param>
void ASTileManager::ConnectDoorBranch(ASTile* TileToAdd, int prevDirection)
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

//pass in tile, check which side connects to path
int ASTileManager::CheckPathSide(ASTile* TileToCheck)
{
	//needs to be random
	TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };

	DirectionsToCheck = Reshuffle2(DirectionsToCheck);

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
	DirectionsToCheck = Reshuffle2(DirectionsToCheck);
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
/// Dylan Log
/// 
/// - Remakes our possible available tiles for path generation purposes
/// </summary>
void ASTileManager::MakeAvailableTiles()
{
	for (int TileC = 0; TileC < AllActiveTiles.Num() - 1; TileC++)
	{
		ASTile* CurrentTile = AllActiveTiles[TileC];

		//check each neighbor
		if (!CurrentTile->IsBossTile())
		{
			if (CurrentTile->HasValidUpNeighbor() && !AvailableTiles.Contains(CurrentTile->UpNeighbor))
			{
				AvailableTiles.Add(CurrentTile->UpNeighbor);
			}
			if (CurrentTile->HasValidDownNeighbor() && !AvailableTiles.Contains(CurrentTile->DownNeighbor))
			{
				AvailableTiles.Add(CurrentTile->DownNeighbor);
			}
			if (CurrentTile->HasValidRightNeighbor() && !AvailableTiles.Contains(CurrentTile->RightNeighbor))
			{
				AvailableTiles.Add(CurrentTile->RightNeighbor);
			}
			if (CurrentTile->HasValidLeftNeighbor() && !AvailableTiles.Contains(CurrentTile->LeftNeighbor))
			{
				AvailableTiles.Add(CurrentTile->LeftNeighbor);
			}
		}
	}
}

/// <summary>
/// Dylan Loe
/// 
/// - Remove all inactive doors
/// </summary>
void ASTileManager::FinalDoorSetupDoors()
{
	for (int doorIndex = 0; doorIndex < DoorArray.Num(); doorIndex++)
	{
		//remove all doors inactive (aka the ones not connecting paths)
		ASTileDoor* door = DoorArray[doorIndex];
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
void ASTileManager::DeactiveInactiveRooms()
{
	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Removing Unwanted tiles..."));

	for (FMultiTileStruct* row : Grid2DArray)
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
/// Knuth shuffle algorithm
/// 
/// - Reshuffling arrays
/// </summary>
/// <param name="ar"> Array input to be reshuffles</param>
/// <returns></returns>
TArray <int> ASTileManager::Reshuffle2(TArray <int> ar)
{
	// Knuth shuffle algorithm :: courtesy of Wikipedia :)
	for (int t = 0; t < ar.Num(); t++)
	{
		int r = GameStream.RandRange(t, ar.Num() - 1);
		ar.Swap(t, r);
	}
	return ar;
}

/// <summary>
/// Gets current grid density
/// </summary>
/// <returns></returns>
float ASTileManager::GetCurrentGridDensity()
{
	return (float)AllActiveTiles.Num() / (float)(LevelHeight * LevelWidth);
}

/// <summary>
/// Dylan Loe
/// 
/// ASSIGNING THE NEIGHBORS AND LINKING DOORS PER TILE
/// connect this tile with the tiles to the left and below
/// must have a HeightIndex less than us and greater than or = to 0
/// must have a WidthIndex less than us and greater than or = to 0
/// This is setting the top and left for each tile (if everyone does it, everything thing gets linked 
/// (instead of making each one have 4 and do an extra pass to prune them))
/// </summary>
/// <param name="ThisTile"> Current Tile being Linked</param>
/// <param name="Col"></param>
void ASTileManager::LinkTile(ASTile* ThisTile, FMultiTileStruct Col)
{
	if (ThisTile->ZIndex > 0)
	{
		//assign this tiles up neighbor
		ThisTile->UpNeighbor = Col.TileColumn[ThisTile->ZIndex - 1];
		ASTile* UpNeighbor = ThisTile->UpNeighbor;
		//assign that tiles down neighbor to be this tile
		Col.TileColumn[ThisTile->ZIndex - 1]->DownNeighbor = ThisTile;

		//if doors are active, spawn a door at the tile placeholder
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//if our lower neighbor doesn't already have a door connecting up
		//TODO: may need work here
		if (!UpNeighbor->DownDoor)
		{
			const FString TileUpDoorName = "TileDoorConnecting_" + FString::FromInt(ThisTile->XIndex) + "_" + FString::FromInt(ThisTile->ZIndex) + "_to_" +
				FString::FromInt(UpNeighbor->XIndex) + "_" + FString::FromInt(UpNeighbor->ZIndex);

			const FVector UpDoorSpawnLocation = ThisTile->UpDoorSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
			const FTransform Spawm = FTransform(ThisTile->UpDoorSpawnPoint.GetRotation(), UpDoorSpawnLocation);

			ThisTile->UpDoor = GetWorld()->SpawnActor<ASTileDoor>(TileDoor, Spawm, SpawnParams);
			DoorArray.Add(ThisTile->UpDoor);
			ThisTile->UpDoor->SetActorLabel(TileUpDoorName);
			ThisTile->UpDoor->SetOwner(ThisTile);
#if WITH_EDITOR
			ThisTile->UpDoor->SetFolderPath(DoorSubFolderName);
#endif
			UpNeighbor->DownDoor = ThisTile->UpDoor;
		}
	}
	if (ThisTile->XIndex > 0)
	{
		//assign this tiles left neighbor from grid and save it locally
		ASTile* LeftNeighbor = ThisTile->LeftNeighbor = Grid2DArray[ThisTile->XIndex - 1]->TileColumn[ThisTile->ZIndex];
		//save that tiles right neighbor to be this tile
		Grid2DArray[ThisTile->XIndex - 1]->TileColumn[ThisTile->ZIndex]->RightNeighbor = ThisTile;

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//if our lower neighbor doesn't already have a door connecting up
		if (!LeftNeighbor->RightDoor)
		{
			FString TileLeftDoorName = "TileDoorConnecting_" + FString::FromInt(ThisTile->XIndex) + "_" + FString::FromInt(ThisTile->ZIndex) + "_to_" +
				FString::FromInt(LeftNeighbor->XIndex) + "_" + FString::FromInt(LeftNeighbor->ZIndex);

			//add LeftDoorSpawnPoint location to ThisTiles location to give world location
			FVector LeftDoorSpawnLocation = ThisTile->LeftDoorSpawnPoint.GetLocation() + ThisTile->GetActorLocation();

			const FTransform Spawm = FTransform(ThisTile->LeftDoorSpawnPoint.GetRotation(), LeftDoorSpawnLocation);

			ThisTile->LeftDoor = GetWorld()->SpawnActor<ASTileDoor>(TileDoor, Spawm, SpawnParams);
			DoorArray.Add(ThisTile->LeftDoor);
			ThisTile->LeftDoor->SetActorLabel(TileLeftDoorName);
			ThisTile->LeftDoor->SetOwner(ThisTile);
#if WITH_EDITOR
			ThisTile->LeftDoor->SetFolderPath(DoorSubFolderName);
#endif
			LeftNeighbor->RightDoor = ThisTile->LeftDoor;
		}
	}
}

// Called every frame
void ASTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//For Spawning doors to attach to tiles
void ASTileManager::SpawnDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect, FTransform doorAdjustment)
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


	ASTileDoor* door = GetWorld()->SpawnActor<ASTileDoor>(TileDoor, Spawm, SpawnParams);

	SetupDoor(tile, SideToSpawnDoor, NameOfTileToConnect, door);

}

void ASTileManager::SetupDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect, ASTileDoor* door)
{
	const FString TileDoorName = "TileDoorConnecting_" + FString::FromInt(tile->XIndex) + "_" + FString::FromInt(tile->ZIndex) + "_to_SecretRoom";

	door->DoorActive = true;
	DoorArray.Add(door);
	door->SetActorLabel(TileDoorName);
	door->SetOwner(this);
#if WITH_EDITOR
	door->SetFolderPath(DoorSubFolderName);
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
/// scales the density factor based on total area. TODO: Maybe this can scale with Difficulty?
/// 
/// Sparse Maze: 0.05f;
/// Moderate Complexity: 0.15f;
/// Highly Branched Maze: 0.25f or higher
/// </summary>
/// <returns></returns>
float ASTileManager::BranchDensityFactor_DependencyOnMazeSize()
{
	if(LevelWidth <= 0 && LevelHeight <= 0)
		UE_LOG(LogTemp, Error, TEXT("Error: No valid grid width and height to determine branch density"));
	float branchDensityFactor = FMath::Clamp(0.1f + (LevelWidth * LevelHeight /60.0f), 0.05f, 0.2f); //(LevelWidth * LevelHeight /x.0f) where x is some arbituary scaling factor (magic number) i need to adjust

	return branchDensityFactor;
}

/// <summary>
///  ensures that as the path expands, the branch factor scales to avoid excessive isolation.
/// </summary>
/// <returns></returns>
float ASTileManager::BranchDensityFactor_DynamicMainPathLength()
{
	if (LevelWidth <= 0 && LevelHeight <= 0)
		UE_LOG(LogTemp, Error, TEXT("Error: No valid grid width and height to determine branch density"));

	//how much density is left after path?
	//float remainingGridDensity = 1.0f - ((float)LevelPath.Num() / ((float)LevelWidth * (float)LevelHeight));
	//UE_LOG(LogTemp, Log, TEXT("Choice 1.5: %f"), remainingGridDensity);
	float branchDensityFactor = FMath::Clamp((LevelPath.Num() / (float)LevelWidth), 0.05f, 0.2f);

	return branchDensityFactor;
}

