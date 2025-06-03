// Fill out your copyright notice in the Description page of Project Settings.


#include "UTilePathSetupComp.h"
#include "Components/ActorComponent.h"
#include "Enviornment/TileGeneration/STileManager.h"

// Sets default values
UTilePathSetupComp::UTilePathSetupComp()
{
 	
	

}

/// <summary>
/// Creates path from starting room to end room
/// 
/// broadcasts event to run next setup phase
/// </summary>
void UTilePathSetupComp::TilePathGeneration()
{
	//get ref to manager
	TileManagerRef = Cast<ASTileManager>(GetOwner());
	UE_LOG(LogTemp, Log, TEXT("-----------------------------------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("================= PATH GENERATION ========================="));
	UE_LOG(LogTemp, Log, TEXT("-----------------------------------------------------------"));
	
	//once tiles are established, branches and random room additions
	ChooseStartEndRooms();
	
	GeneratePath();

	CreateSpawnRoom();

	//notify next component to run
	OnPathGeneratedEvent.Broadcast();
}

// Called when the game starts or when spawned
void UTilePathSetupComp::BeginPlay()
{
	Super::BeginPlay();
	
}

/// <summary>
/// Dylan Loe
/// 
/// Choosing the start and end room
/// 
/// TO DO: Do i really need to pick the end room?
/// 
/// </summary>
void UTilePathSetupComp::ChooseStartEndRooms()
{
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Choosing start and end rooms =============================="));

	int startX = 0, startY = 0;
	//will pick a random side and random tile on side to start
	if (StartRoomSide == -1)
		StartRoomSide = TileManagerRef->GameStream.RandRange(0, 3);
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Side Picked: %d"), StartRoomSide);

	//refs to static vars from manager
	TArray <FMultiTileStruct*> Grid2DArrayRef = TileManagerRef->Grid2DArray;
	int levelWidthRef = TileManagerRef->GetLevelWidth();
	int levelHeightRef = TileManagerRef->GetLevelHeight();

	switch (StartRoomSide) {
	case 0:
		//starting
		startY = 0;
		startX = TileManagerRef->GameStream.RandRange(0, levelHeightRef - 1);
		//UE_LOG(LogTemp, Log, TEXT("num Picked: %d"), startX);
		//DOWN;

		//ending possible 
		//opposite of startX (0) would be LevelWidth - 1
		//for choosing the end tile, we will pick the opposite 2 rows/columns
		//must be a distance of height/2 and/or column/2 to be added to a list to be randomly picked as end room
		//cant pick tiles that are on the same row or column (no straight shots)
		for (int index2 = 0; index2 < (levelWidthRef - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startX; index++)
			{
				//ASTile* Possible = Grid2DArray[LevelWidth - 1]->TileColumn[index];
				TileManagerRef->PossibleStartingTiles.Add(TileManagerRef->GetGridTile(levelWidthRef - 1 - index2,index));
				TileManagerRef->GetGridTile(levelWidthRef - 1 - index2,index)->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = levelHeightRef - 1; index > startX; index--)
			{
				TileManagerRef->PossibleStartingTiles.Add(TileManagerRef->GetGridTile(levelWidthRef - 1 - index2,index));
				TileManagerRef->GetGridTile(levelWidthRef - 1 - index2, index)->ShadeTestRoom();
			}
		}

		//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
		if (startX > (levelWidthRef - 1) / 2) { //more than half AFTER startX
			for (int extra = 0; extra < (startX)-((levelHeightRef) / 2); extra++) {
				for (int index3 = 0; index3 < levelHeightRef - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index3, extra);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[levelHeightRef - 1 - index3]->TileColumn[extra]);
					Grid2DArrayRef[levelHeightRef - 1 - index3]->TileColumn[extra]->ShadeTestRoom();
				}
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
		if (startX < ((levelWidthRef - 1) / 2)) { //more than after BEFORE startX
			for (int extra = 0; extra < (levelHeightRef - startX - 1) - ((levelHeightRef) / 2); extra++) {
				for (int index4 = 0; index4 < levelHeightRef - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index4, LevelWidth - 1 - extra);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[levelHeightRef - 1 - index4]->TileColumn[levelWidthRef - 1 - extra]);
					Grid2DArrayRef[levelHeightRef - 1 - index4]->TileColumn[levelWidthRef - 1 - extra]->ShadeTestRoom();
				}
			}
		}


		break;
	case 1:
		startY = TileManagerRef->GameStream.RandRange(0, levelWidthRef - 1);
		startX = 0;
		//LEFT;

		for (int index2 = 0; index2 < (levelHeightRef - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startY; index++)
			{
				//ASTile* Possible = Grid2DArray[LevelWidth - 1]->TileColumn[index];
				TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[index]->TileColumn[levelHeightRef - 1 - index2]);
				Grid2DArrayRef[index]->TileColumn[levelHeightRef - 1 - index2]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = levelHeightRef - 1; index > startY; index--)
			{
				TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[index]->TileColumn[levelHeightRef - 1 - index2]);
				Grid2DArrayRef[index]->TileColumn[levelHeightRef - 1 - index2]->ShadeTestRoom();
			}
		}


		//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d < %d"), startY, (LevelHeight - 1) / 2);
		if (startY > (levelHeightRef - 1) / 2) { //more than half BEFORE startX
			//UE_LOG(LogTemp, Log, TEXT("Check: %d"), (startY)-((LevelWidth) / 2));
			for (int extra = 0; extra < (startY)-((levelWidthRef) / 2); extra++) {
				for (int index3 = 0; index3 < levelWidthRef - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), extra, LevelWidth - 1 - index3);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[extra]->TileColumn[levelWidthRef - 1 - index3]);
					Grid2DArrayRef[extra]->TileColumn[levelWidthRef - 1 - index3]->ShadeTestRoom();
				}
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d > %d"), startY, (LevelHeight - 1) / 2);
		if (startY < ((levelHeightRef - 1) / 2)) { //more than after AFTER startX
			//UE_LOG(LogTemp, Log, TEXT("Check: %d"), (LevelWidth - startY - 1) - ((LevelWidth) / 2));
			for (int extra = 0; extra < (levelWidthRef - startY - 1) - ((levelWidthRef) / 2); extra++) {
				for (int index4 = 0; index4 < levelWidthRef - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - extra, LevelWidth - 1 - index4);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[levelHeightRef - 1 - extra]->TileColumn[levelWidthRef - 1 - index4]);
					Grid2DArrayRef[levelHeightRef - 1 - extra]->TileColumn[levelWidthRef - 1 - index4]->ShadeTestRoom();
				}
			}
		}


		break;
	case 2:
		startY = levelWidthRef - 1;
		startX = TileManagerRef->GameStream.RandRange(0, levelHeightRef - 1);
		//UP;

		for (int index2 = 0; index2 < (levelWidthRef - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startX; index++)
			{
				TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[0 + index2]->TileColumn[index]);
				Grid2DArrayRef[0 + index2]->TileColumn[index]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = levelHeightRef - 1; index > startX; index--)
			{
				TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[0 + index2]->TileColumn[index]);
				Grid2DArrayRef[0 + index2]->TileColumn[index]->ShadeTestRoom();
			}
		}

		//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
		if (startX > (levelWidthRef - 1) / 2) { //more than half AFTER startX
			for (int extra = 0; extra < (startX)-((levelHeightRef) / 2); extra++) {
				for (int index3 = 0; index3 < levelHeightRef - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), index3, extra);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[index3]->TileColumn[extra]);
					Grid2DArrayRef[index3]->TileColumn[extra]->ShadeTestRoom();
				}
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
		if (startX < ((levelWidthRef - 1) / 2)) { //more than after BEFORE startX
			for (int extra = 0; extra < (levelHeightRef - startX - 1) - ((levelHeightRef) / 2); extra++) {
				for (int index4 = 0; index4 < levelHeightRef - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), index4, LevelWidth - 1 - extra);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[index4]->TileColumn[levelWidthRef - 1 - extra]);
					Grid2DArrayRef[index4]->TileColumn[levelWidthRef - 1 - extra]->ShadeTestRoom();
				}
			}
		}

		break;
	case 3:
		startY = TileManagerRef->GameStream.RandRange(0, levelHeightRef - 1);
		startX = levelWidthRef - 1;
		//RIGHT;

		for (int index2 = 0; index2 < (levelHeightRef - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startY; index++)
			{
				TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[index]->TileColumn[index2]);
				Grid2DArrayRef[index]->TileColumn[index2]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = levelWidthRef - 1; index > startY; index--)
			{
				TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[index]->TileColumn[index2]);
				Grid2DArrayRef[index]->TileColumn[index2]->ShadeTestRoom();
			}
		}
		if (startY > (levelHeightRef - 1) / 2) { //more than half BEFORE startX
			for (int extra = 0; extra < (startY)-((levelWidthRef) / 2); extra++) {
				//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startY, (LevelHeight - 1) / 2);

				for (int index3 = 0; index3 < levelHeightRef - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), extra, index3);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[extra]->TileColumn[index3]);
					Grid2DArrayRef[extra]->TileColumn[index3]->ShadeTestRoom();
				}
			}
		}
		if (startY < ((levelHeightRef - 1) / 2)) { //more than after AFTER startX
			for (int extra = 0; extra < (levelWidthRef - startY - 1) - ((levelWidthRef) / 2); extra++) {
				//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startY, (LevelHeight - 1) / 2);
				for (int index4 = 0; index4 < levelHeightRef - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"),  LevelWidth - 1 - extra, index4);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[levelWidthRef - 1 - extra]->TileColumn[index4]);
					Grid2DArrayRef[levelWidthRef - 1 - extra]->TileColumn[index4]->ShadeTestRoom();
				}
			}
		}

		break;
	default:
		if (TileManagerRef->DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Issue here -> picked weird starting side in ChooseStartEndRooms"));
		//startY = 0;
		//startX = GameStream.RandRange(0, LevelHeight - 1);
		//DOWN	
		for (int index2 = 0; index2 < (levelWidthRef - 1) / 2; index2++) {
			//take every tile less than startY
			for (int index = 0; index < startX; index++)
			{
				//ASTile* Possible = Grid2DArray[LevelWidth - 1]->TileColumn[index];
				TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[levelWidthRef - 1 - index2]->TileColumn[index]);
				Grid2DArrayRef[levelWidthRef - 1 - index2]->TileColumn[index]->ShadeTestRoom();
			}

			//take every tile greater than startY
			for (int index = levelHeightRef - 1; index > startX; index--)
			{
				TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[levelWidthRef - 1 - index2]->TileColumn[index]);
				Grid2DArrayRef[levelWidthRef - 1 - index2]->TileColumn[index]->ShadeTestRoom();
			}
		}

		//UE_LOG(LogTemp, Log, TEXT("Compare1 BEFORE: %d > %d"), startX, (LevelWidth - 1) / 2);
		if (startX > (levelWidthRef - 1) / 2) { //more than half AFTER startX
			for (int extra = 0; extra < (startX)-((levelHeightRef) / 2); extra++) {
				for (int index3 = 0; index3 < levelHeightRef - 1; index3++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index3, extra);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[levelHeightRef - 1 - index3]->TileColumn[extra]);
					Grid2DArrayRef[levelHeightRef - 1 - index3]->TileColumn[extra]->ShadeTestRoom();
				}
			}
		}
		//UE_LOG(LogTemp, Log, TEXT("Compare2 AFTER: %d < %d"), startX, (LevelWidth - 1) / 2);
		if (startX < ((levelWidthRef - 1) / 2)) { //more than after BEFORE startX
			for (int extra = 0; extra < (levelHeightRef - startX - 1) - ((levelHeightRef) / 2); extra++) {
				for (int index4 = 0; index4 < levelHeightRef - 1; index4++)
				{
					//UE_LOG(LogTemp, Log, TEXT("Tile: %d, %d"), LevelHeight - 1 - index4, LevelWidth - 1 - extra);
					TileManagerRef->PossibleStartingTiles.Add(Grid2DArrayRef[levelHeightRef - 1 - index4]->TileColumn[levelWidthRef - 1 - extra]);
					Grid2DArrayRef[levelHeightRef - 1 - index4]->TileColumn[levelWidthRef - 1 - extra]->ShadeTestRoom();
				}
			}
		}

		break;
	}
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Starting Tile is designated as [%d,%d]"), startY, startX);

	StartingTile = TileManagerRef->GetGridTile(startY, startX);//Grid2DArray[startY]->TileColumn[startX];
	StartingTile->ShadeStartingRoom();

	//end room is picked randomly from array of possible rooms
	TArray<ASTile*> availableTiles = TileManagerRef->GetPossibleStartingTiles();
	EndTile = availableTiles[TileManagerRef->GameStream.RandRange(0, availableTiles.Num() - 1)];
	EndTile->ShadeEndRoom();

	
}

void UTilePathSetupComp::GeneratePath()
{
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Genearating Path =============================="));

	//add starting room to be start of list
	//AddTileToPath(StartingTile);

	//refs from manager
	//TArray<ASTile*> LevelPathRef = TileManagerRef->LevelPath;

	CheckTile(StartingTile, TileManagerRef->LevelPath);

	if (TileManagerRef->DebugPrints) {
		//draw lines through path
		for (int Index = 0; Index < TileManagerRef->LevelPath.Num() - 1; Index++)
		{
			DrawDebugLine(GetWorld(), TileManagerRef->LevelPath[Index]->GetActorLocation(), TileManagerRef->LevelPath[Index + 1]->GetActorLocation(), FColor::Blue, SDPG_World, 20.0f, 150);
			//GetWorld()->LineBatcher->DrawLine(LevelPath[Index]->GetActorLocation(), LevelPath[Index + 1]->GetActorLocation(), FColor::Blue, SDPG_World, 10.0f, 100);
		}
	}

}

/// <summary>
/// Dylan Loe
/// 
/// - Add spawn room, connected to  the start room (this will be outside of the grid
/// </summary>
void UTilePathSetupComp::CreateSpawnRoom()
{
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Creating Starting Spawn Room..."));

	FVector SpawnPos;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALocalLevel* MyLocalLevelRef = TileManagerRef->MyLocalLevel;

	//There are going to be 2 tiles basically spawned, one is the base, the base structure of the tile
	// The other tile (which i don't think needs to be a tile at all), is the environment to be populated on the tile
	// Since the environment will be rotated, its neighbors will be changed theretofore for now it will be faster to have its own base that stays static)
	switch (StartRoomSide)
	{
	case 0:
		//RIGHT
		SpawnPos = FVector(StartingTile->GetActorLocation().X - (StartingTile->TileLength), StartingTile->GetActorLocation().Y, StartingTile->GetActorLocation().Z);
		PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		PlayerStartingTile_SpawnedPreset = GetWorld()->SpawnActor<ASTile>(MyLocalLevelRef->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams); //rotate -90
		StartingTile->LeftNeighbor = PlayerStartingTileBase;
		PlayerStartingTileBase->RightNeighbor = StartingTile;
		PlayerStartingTile_SpawnedPreset->SetActorRotation(FRotator(PlayerStartingTile_SpawnedPreset->GetActorRotation().Euler().X, -90.0f, PlayerStartingTile_SpawnedPreset->GetActorRotation().Euler().Z));
		UE_LOG(LogTemp, Log, TEXT("No door spawned in start room, add code if we use this route..."));

		break;
	case 1:
		//UP
		SpawnPos = FVector(StartingTile->GetActorLocation().X, StartingTile->GetActorLocation().Y - (StartingTile->TileLength), StartingTile->GetActorLocation().Z);
		PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		PlayerStartingTile_SpawnedPreset = GetWorld()->SpawnActor<ASTile>(MyLocalLevelRef->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		StartingTile->UpNeighbor = PlayerStartingTileBase;
		PlayerStartingTileBase->DownNeighbor = StartingTile;
		UE_LOG(LogTemp, Log, TEXT("No door spawned in start room, add code if we use this route..."));

		break;
	case 2:
		//LEFT
		SpawnPos = FVector(StartingTile->GetActorLocation().X + (StartingTile->TileLength), StartingTile->GetActorLocation().Y, StartingTile->GetActorLocation().Z);
		PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		PlayerStartingTile_SpawnedPreset = GetWorld()->SpawnActor<ASTile>(MyLocalLevelRef->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams); //rotate 90
		StartingTile->RightNeighbor = PlayerStartingTileBase;
		PlayerStartingTileBase->LeftNeighbor = StartingTile;
		PlayerStartingTile_SpawnedPreset->SetActorRotation(FRotator(PlayerStartingTile_SpawnedPreset->GetActorRotation().Euler().X, 90, PlayerStartingTile_SpawnedPreset->GetActorRotation().Euler().Z));
		UE_LOG(LogTemp, Log, TEXT("No door spawned in start room, add code if we use this route..."));

		break;
	case 3:
		//DOWN
		SpawnPos = FVector(StartingTile->GetActorLocation().X, StartingTile->GetActorLocation().Y + (StartingTile->TileLength), StartingTile->GetActorLocation().Z);
		PlayerStartingTileBase = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, StartingTile->GetActorRotation(), SpawnParams); //rotate 180
		PlayerStartingTile_SpawnedPreset = GetWorld()->SpawnActor<ASTile>(MyLocalLevelRef->PresetStartingTile, SpawnPos, StartingTile->GetActorRotation(), SpawnParams);
		StartingTile->DownNeighbor = PlayerStartingTileBase;
		PlayerStartingTileBase->UpNeighbor = StartingTile;
		PlayerStartingTile_SpawnedPreset->SetActorRotation(FRotator(PlayerStartingTile_SpawnedPreset->GetActorRotation().Euler().X, 180.0f, PlayerStartingTile_SpawnedPreset->GetActorRotation().Euler().Z));

		if (TileManagerRef->DoorToStartRoom)
		{
			//Set up door - rn defaulted to 3 TODO: do i really need to even choose other sides to start on? is the player even gunna notice? perspective of player always starts the same
			//should always be having the same rotation right? therefore the door should always be up while everyone else's has different orientations
			const FString TileUpDoorName = "TileDoorConnecting_StartingRoom_to_" + FString::FromInt(PlayerStartingTileBase->UpNeighbor->XIndex) + "_" + FString::FromInt(PlayerStartingTileBase->UpNeighbor->ZIndex);
			const FVector UpDoorSpawnLocation = PlayerStartingTileBase->UpDoorSpawnPoint.GetLocation() + PlayerStartingTileBase->GetActorLocation();
			const FTransform Spawm = FTransform(PlayerStartingTileBase->UpDoorSpawnPoint.GetRotation(), UpDoorSpawnLocation);
			PlayerStartingTileBase->UpDoor = GetWorld()->SpawnActor<ASTileDoor>(TileManagerRef->TileDoor, Spawm, SpawnParams);
			TileManagerRef->DoorArray.Add(PlayerStartingTileBase->UpDoor);
			PlayerStartingTileBase->UpDoor->SetActorLabel(TileUpDoorName);
			PlayerStartingTileBase->UpDoor->SetOwner(StartingTile);

#if WITH_EDITOR
			PlayerStartingTileBase->UpDoor->SetFolderPath(TileManagerRef->DoorSubFolderName);
#endif
			StartingTile->DownDoor = PlayerStartingTileBase->UpDoor;
			//PlayerStartingTileBase->ActivateUpDoor();

			StartingTile->ConnectDownDoor();
		}
		break;
	}

	//label
	PlayerStartingTileBase->SetActorLabel("StartingTile_Base");
	PlayerStartingTile_SpawnedPreset->SetActorLabel("StartingTile_Populate");
#if WITH_EDITOR
	PlayerStartingTileBase->SetFolderPath(TileManagerRef->TileGenRootFolder);
	PlayerStartingTile_SpawnedPreset->SetFolderPath(TileManagerRef->TileGenRootFolder);
#endif
	//Set the Preset ref to the SpawnPresetTile obj
	PlayerStartingTileBase->PresetTile = PlayerStartingTile_SpawnedPreset;

	//Spawn stats from StartingTile to PlayerStartingTileBase, then we will reassign the StartingTile
	StartingTile->ShadePath();
	TileManagerRef->LevelPath.Insert(PlayerStartingTileBase, 0);
	TileManagerRef->AllActiveTiles.Insert(PlayerStartingTileBase, 0);
	PlayerStartingTileBase->ShadeStartingRoom();
	StartingTile = PlayerStartingTileBase;
}

/// <summary>
/// Backtracking reclusive algorithm for Main level path construction. Builds out LevelPath array.
/// TODO: As we go, doors connecting tiles will be marked, after everything is done, unmarked doors get destroyed at the end (no need to have second pass checking)
/// </summary>
/// <param name="CurrentTile"></param>
/// <param name="CurrentPath"></param>
void UTilePathSetupComp::CheckTile(ASTile* CurrentTile, TArray<ASTile*>& CurrentPath)
{
	bool CheckingTileDebug = false;

	int levelWidthRef = TileManagerRef->GetLevelWidth();
	int levelHeightRef = TileManagerRef->GetLevelHeight();

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

	if (FailsafeCount == levelHeightRef * levelWidthRef * 2)
	{
		UE_LOG(LogTemp, Log, TEXT("NHitting dead ends. PLEASE INVESTIGATE"));
	}
	else {
		FailsafeCount++;
	}

	//first check if all neighbors are unavailable
	//if so, make this one as checked and call on previous tiles

	//check all neighbors
	ASTile* neighbors[] = { CurrentTile->UpNeighbor, CurrentTile->DownNeighbor, CurrentTile->RightNeighbor, CurrentTile->LeftNeighbor }; //could be local param but we dont use this very much
	bool allNeighborsInvalid = true;
	for (ASTile* n : neighbors)
	{
		//check if invalid or not part of path
		if (n && !n->CheckForPath)
		{
			allNeighborsInvalid = false;
			break;
		}
	}

	if (allNeighborsInvalid)
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

bool UTilePathSetupComp::AddTileToPath(ASTile* TileToAdd)
{
	TileManagerRef->PathNumber++;
	TileManagerRef->LevelPath.AddUnique(TileToAdd);
	TileToAdd->CheckForPath = true;
	TileToAdd->PathNumber = TileManagerRef->PathNumber;
	if (!TileToAdd->IsBossTile() && !TileToAdd->IsStartingTile())
		TileToAdd->ShadePath();

	return true;
}

/// <summary>
/// Knuth shuffle algorithm
/// 
/// - Reshuffling arrays
/// </summary>
/// <param name="ar"> Array input to be reshuffles</param>
/// <returns></returns>
TArray <int> UTilePathSetupComp::Reshuffle2(TArray <int> ar)
{
	// Knuth shuffle algorithm :: courtesy of Wikipedia :)
	for (int t = 0; t < ar.Num(); t++)
	{
		int r = TileManagerRef->GameStream.RandRange(t, ar.Num() - 1);
		ar.Swap(t, r);
	}
	return ar;
}


