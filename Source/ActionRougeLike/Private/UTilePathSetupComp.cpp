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

	GridScanForCustomTileSizedVariants();

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

	TileManagerRef->StartingGridTile = TileManagerRef->GetGridTile(startY, startX);//Grid2DArray[startY]->TileColumn[startX];
	TileManagerRef->StartingGridTile->ShadeStartingRoom();

	//end room is picked randomly from array of possible rooms
	TArray<ASTile*> availableTiles = TileManagerRef->GetPossibleStartingTiles();
	TileManagerRef->EndTile = availableTiles[TileManagerRef->GameStream.RandRange(0, availableTiles.Num() - 1)];
	TileManagerRef->EndTile->ShadeEndRoom();

	
}

void UTilePathSetupComp::GeneratePath()
{
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== Genearating Path =============================="));

	//add starting room to be start of list
	//AddTileToPath(StartingTile);

	//refs from manager
	//TArray<ASTile*> LevelPathRef = TileManagerRef->LevelPath;

	CheckTile(TileManagerRef->StartingGridTile, TileManagerRef->LevelPath);

	if (TileManagerRef->DebugPrints) {
		//draw lines through path
		for (int Index = 0; Index < TileManagerRef->LevelPath.Num() - 1; Index++)
		{
			DrawDebugLine(GetWorld(), TileManagerRef->LevelPath[Index]->GetActorLocation(), TileManagerRef->LevelPath[Index + 1]->GetActorLocation(), FColor::Blue, SDPG_World, 20.0f, 150);
			//GetWorld()->LineBatcher->DrawLine(LevelPath[Index]->GetActorLocation(), LevelPath[Index + 1]->GetActorLocation(), FColor::Blue, SDPG_World, 10.0f, 100);
		}
	}
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Main Path Length: %d"), TileManagerRef->LevelPath.Num());
}

/// <summary>
/// Dylan Loe
/// 
/// - Add spawn room, connected to  the start room (this will be outside of the grid)
/// </summary>
void UTilePathSetupComp::CreateSpawnRoom()
{
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Creating Starting Spawn Room..."));

	FVector SpawnPos;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ALocalLevel* MyLocalLevelRef = TileManagerRef->MyLocalLevel;
	ASTile* StartingGridTileRef = TileManagerRef->StartingGridTile;

	TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAssetRef = TileManagerRef->ChoosenDoorwayAsset;
	FName WallsSubFolderNameRef = TileManagerRef->WallsSubFolderName;

	float DoorwayAdjustment = TileManagerRef->ChoosenWallAsset->GetDefaultObject<ASTileWall>()->WallsBuffer;

	//There are going to be 2 tiles basically spawned, one is the base, the base structure of the tile
	// The other tile (which i don't think needs to be a tile at all), is the environment to be populated on the tile
	// Since the environment will be rotated, its neighbors will be changed theretofore for now it will be faster to have its own base that stays static)
	switch (StartRoomSide)
	{
	case 0:
		//RIGHT
		SpawnPos = FVector(StartingGridTileRef->GetActorLocation().X - (StartingGridTileRef->TileLength + DoorwayAdjustment), StartingGridTileRef->GetActorLocation().Y, StartingGridTileRef->GetActorLocation().Z);
		TileManagerRef->PlayerStartingTile_SpawnTile = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, StartingGridTileRef->GetActorRotation(), SpawnParams);
		TileManagerRef->PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevelRef->PresetStartingTile, SpawnPos, StartingGridTileRef->GetActorRotation(), SpawnParams); //rotate -90
		StartingGridTileRef->LeftNeighbor = TileManagerRef->PlayerStartingTile_SpawnTile;
		TileManagerRef->PlayerStartingTile_SpawnTile->RightNeighbor = StartingGridTileRef;
		TileManagerRef->PlayerSpawnPresentTile->SetActorRotation(FRotator(TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().X, -90.0f, TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().Z));

		break;
	case 1:
		//UP
		SpawnPos = FVector(StartingGridTileRef->GetActorLocation().X, StartingGridTileRef->GetActorLocation().Y - (StartingGridTileRef->TileLength + DoorwayAdjustment), StartingGridTileRef->GetActorLocation().Z);
		TileManagerRef->PlayerStartingTile_SpawnTile = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, StartingGridTileRef->GetActorRotation(), SpawnParams);
		TileManagerRef->PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevelRef->PresetStartingTile, SpawnPos, StartingGridTileRef->GetActorRotation(), SpawnParams);
		StartingGridTileRef->UpNeighbor = TileManagerRef->PlayerStartingTile_SpawnTile;
		TileManagerRef->PlayerStartingTile_SpawnTile->DownNeighbor = StartingGridTileRef;

		break;
	case 2:
		//LEFT
		SpawnPos = FVector(StartingGridTileRef->GetActorLocation().X + (StartingGridTileRef->TileLength + DoorwayAdjustment), StartingGridTileRef->GetActorLocation().Y, StartingGridTileRef->GetActorLocation().Z);
		TileManagerRef->PlayerStartingTile_SpawnTile = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, StartingGridTileRef->GetActorRotation(), SpawnParams);
		TileManagerRef->PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevelRef->PresetStartingTile, SpawnPos, StartingGridTileRef->GetActorRotation(), SpawnParams); //rotate 90
		StartingGridTileRef->RightNeighbor = TileManagerRef->PlayerStartingTile_SpawnTile;
		TileManagerRef->PlayerStartingTile_SpawnTile->LeftNeighbor = StartingGridTileRef;
		TileManagerRef->PlayerSpawnPresentTile->SetActorRotation(FRotator(TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().X, 90, TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().Z));

		break;
	case 3:
		//DOWN
		SpawnPos = FVector(StartingGridTileRef->GetActorLocation().X, StartingGridTileRef->GetActorLocation().Y + (StartingGridTileRef->TileLength + DoorwayAdjustment), StartingGridTileRef->GetActorLocation().Z);
		TileManagerRef->PlayerStartingTile_SpawnTile = GetWorld()->SpawnActor<ASTile>(TileManagerRef->TileBase, SpawnPos, StartingGridTileRef->GetActorRotation(), SpawnParams); //rotate 180
		TileManagerRef->PlayerSpawnPresentTile = GetWorld()->SpawnActor<ASTile>(MyLocalLevelRef->PresetStartingTile, SpawnPos, StartingGridTileRef->GetActorRotation(), SpawnParams);
		StartingGridTileRef->DownNeighbor = TileManagerRef->PlayerStartingTile_SpawnTile;
		TileManagerRef->PlayerStartingTile_SpawnTile->UpNeighbor = StartingGridTileRef;
		TileManagerRef->PlayerSpawnPresentTile->SetActorRotation(FRotator(TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().X, 180.0f, TileManagerRef->PlayerSpawnPresentTile->GetActorRotation().Euler().Z));

		if (TileManagerRef->DoorToStartRoom)
		{
			//Set up door - rn defaulted to 3 TODO: do i really need to even choose other sides to start on? is the player even gunna notice? perspective of player always starts the same
			//should always be having the same rotation right? therefore the door should always be up while everyone else's has different orientations
			const FString TileUpDoorName = "TileDoorConnecting_StartingRoom_to_" + FString::FromInt(TileManagerRef->PlayerStartingTile_SpawnTile->UpNeighbor->XIndex) + "_" + FString::FromInt(TileManagerRef->PlayerStartingTile_SpawnTile->UpNeighbor->ZIndex);
			const FVector UpDoorSpawnLocation = TileManagerRef->PlayerStartingTile_SpawnTile->UpDoorSpawnPoint.GetLocation() + TileManagerRef->PlayerStartingTile_SpawnTile->GetActorLocation();
			const FTransform Spawm = FTransform(TileManagerRef->PlayerStartingTile_SpawnTile->UpDoorSpawnPoint.GetRotation(), UpDoorSpawnLocation);
			TileManagerRef->PlayerStartingTile_SpawnTile->UpDoor = GetWorld()->SpawnActor<ASTileDoor>(TileManagerRef->TileDoor, Spawm, SpawnParams);
			TileManagerRef->DoorArray.Add(TileManagerRef->PlayerStartingTile_SpawnTile->UpDoor);
			TileManagerRef->PlayerStartingTile_SpawnTile->UpDoor->SetActorLabel(TileUpDoorName);
			TileManagerRef->PlayerStartingTile_SpawnTile->UpDoor->SetOwner(StartingGridTileRef);

#if WITH_EDITOR
			TileManagerRef->PlayerStartingTile_SpawnTile->UpDoor->SetFolderPath(TileManagerRef->DoorSubFolderName);
#endif
			StartingGridTileRef->DownDoor = TileManagerRef->PlayerStartingTile_SpawnTile->UpDoor;
			//PlayerStartingTileBase->ActivateUpDoor();

			StartingGridTileRef->ConnectDownDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		}
		break;
	}

	//label
	TileManagerRef->PlayerStartingTile_SpawnTile->SetActorLabel("StartingTile_Base");
	TileManagerRef->PlayerSpawnPresentTile->SetActorLabel("StartingTile_Populate");
#if WITH_EDITOR
	TileManagerRef->PlayerStartingTile_SpawnTile->SetFolderPath(TileManagerRef->TileGenRootFolder);
	TileManagerRef->PlayerSpawnPresentTile->SetFolderPath(TileManagerRef->TileGenRootFolder);
#endif
	//Set the Preset ref to the SpawnPresetTile obj
	TileManagerRef->PlayerStartingTile_SpawnTile->PresetTile = TileManagerRef->PlayerSpawnPresentTile;

	//Spawn stats from StartingTile to PlayerStartingTileBase, then we will reassign the StartingTile
	TileManagerRef->StartingGridTile->ShadePath();
	TileManagerRef->LevelPath.Insert(TileManagerRef->PlayerStartingTile_SpawnTile, 0);
	TileManagerRef->AllActiveTiles.Insert(TileManagerRef->PlayerStartingTile_SpawnTile, 0);
	TileManagerRef->PlayerStartingTile_SpawnTile->ShadeStartingRoom();
	TileManagerRef->StartingGridTile = TileManagerRef->PlayerStartingTile_SpawnTile;
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
	TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAssetRef = TileManagerRef->ChoosenDoorwayAsset;
	FName WallsSubFolderNameRef = TileManagerRef->WallsSubFolderName;

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

		CurrentTile->ConnectUpDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		doorTransform = CurrentTile->UpDoor->GetTransform();
	}
	else if ((CurrentTile->DownNeighbor && CurrentTile->DownNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->DownNeighbor->XIndex, CurrentTile->DownNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->DownNeighbor);
		CurrentTile->ConnectDownDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		doorTransform = CurrentTile->DownDoor->GetTransform();
	}
	else if ((CurrentTile->RightNeighbor && CurrentTile->RightNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->RightNeighbor->XIndex, CurrentTile->RightNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->RightNeighbor);
		CurrentTile->ConnectRightDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		doorTransform = CurrentTile->RightDoor->GetTransform();
	}
	else if ((CurrentTile->LeftNeighbor && CurrentTile->LeftNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->LeftNeighbor->XIndex, CurrentTile->LeftNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->LeftNeighbor);
		CurrentTile->ConnectLeftDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		doorTransform = CurrentTile->LeftDoor->GetTransform();
	}
	else {
		//now that we know theres valid neighbors and none of them are the boss room, lets check our neighbors

		//UE_LOG(LogTemp, Log, TEXT("Path Checking: %d,%d"), CurrentTile->XIndex, CurrentTile->ZIndex);
		//direction
		TArray <int> DirectionsToCheck = { 1, 2, 3, 4 };

		DirectionsToCheck = Reshuffle(DirectionsToCheck);

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
					CurrentTile->ActivateUpDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
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
					CurrentTile->ActivateDownDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
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
					CurrentTile->ActivateLeftDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
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
					CurrentTile->ActivateRightDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
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
TArray <int> UTilePathSetupComp::Reshuffle(TArray <int> ar)
{
	// Knuth shuffle algorithm :: courtesy of Wikipedia :)
	for (int t = 0; t < ar.Num(); t++)
	{
		int r = TileManagerRef->GameStream.RandRange(t, ar.Num() - 1);
		ar.Swap(t, r);
	}
	return ar;
}

//template be more efficient
TArray <ASTile*> UTilePathSetupComp::ReshuffleTiles(TArray <ASTile*> ar)
{
	// Knuth shuffle algorithm :: courtesy of Wikipedia :)
	for (int t = 0; t < ar.Num(); t++)
	{
		int r = TileManagerRef->GameStream.RandRange(t, ar.Num() - 1);
		ar.Swap(t, r);
	}
	return ar;
}

/// <summary>
/// 
/// </summary>
void UTilePathSetupComp::GridScanForCustomTileSizedVariants()
{
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Starting Custom Sized Tile Variants"));
	//scan through grid where abnormal tiles could potentially be placed
	//only tiles that are off limits would be starting and end tile (TODO: maybe higher tiers of levels could have variants?)
	TArray<ASTile*>	ActiveUnusedTiles = TileManagerRef->AllActiveTiles;


	//TODO: Possible enhancement, maybe we could weight the tiles based on proximity to main path???
	//these candidates will be randomized (shuffle array)

	//each stage
	for (int tileVariantTier = 0; tileVariantTier < TileManagerRef->TileVariantTiers.Num(); tileVariantTier++)
	{
		FTileVariantDefinitionRow tier = TileManagerRef->TileVariantTiers[tileVariantTier];
		//each tier of variants has a certain amount
		int VariantAmount = FMath::RandRange(tier.Min, tier.Max);
		int VariantsPlaced = 0;

		//each type (so 2x2, 4x4, etc) of variant
		for (int tileVariantType = 0; tileVariantType < tier.Columns.Num() && VariantsPlaced < VariantAmount; tileVariantType++)
		{
			//check for the highest priority variant (we work down from there)
			USFTileVariantDefinitionData* currentVariant = tier.Columns[tileVariantType];
			
			//each variant type has a max we can place as well
			int LocalVariantAmount = FMath::RandRange(currentVariant->minorMin, currentVariant->minorMax);
			int localVariantsPlaced = 0;

			//shuffle AllActiveTiles
			ActiveUnusedTiles = ReshuffleTiles(ActiveUnusedTiles);



			//scan in random order
			//for each randomly choosen candidate (a tile on the grid): 
			for (int tileCount = 0; tileCount < ActiveUnusedTiles.Num() && VariantsPlaced < VariantAmount && localVariantsPlaced < LocalVariantAmount; tileCount++)
			{
				ASTile* currentTile = ActiveUnusedTiles[tileCount];
				//candidates analysis, pass in current variant, etc
				VariantCandidateAnalysis(currentTile, currentVariant, LocalVariantAmount, localVariantsPlaced);

			}
		}
	}
}

/// <summary>
/// Variant Candidate Analysis (if we can place variant and place procedure)
/// </summary>
/// <param name="CurrentTile"></param>
/// <param name="CurrentVariant"></param>
/// <param name="totalAmount"></param>
/// <param name="placed"></param>
void UTilePathSetupComp::VariantCandidateAnalysis(ASTile* CurrentTile, USFTileVariantDefinitionData* CurrentVariant, int& totalAmount, int& placed)
{
	//current tile is starting point, then we check every transform for variant to see if it fits

	//check if we can place the variant based on size of variant and availability of tile
	if (!CurrentTile->TileVariantInUse)
	{
		TArray<ASTileDoor*> DoorsArray;
		TArray<ASTileWall*> WallArray;
		int choosenSide = -1;

		for (FVariantOffsetTransforms_Rotates transform : CurrentVariant->VariantPaths)
		{
			TArray <ASTile*> EncompassingTilesBuild;
			//check all offsets based on this main tile starting point and populate corresponding data for setup if it fits!
			if (PlugTile(transform, CurrentVariant, CurrentTile, choosenSide, EncompassingTilesBuild, DoorsArray, WallArray))
			{
				int variantIndex = TileManagerRef->GameStream.RandRange(0, CurrentVariant->TileVariantEnviornments.Num() - 1);
				TSubclassOf<ASTileVariantEnviornment> ChoosenVariant = CurrentVariant->TileVariantEnviornments[variantIndex];
				FVector SpawnPos;
				FRotator SpawnRot = FRotator(0.0f, 0.0f, 0.0f);

				//set rotation of TilePrefab
				float rotationModifier = 0;
				//start with location of currentTile, rotate based on which side choosen
				switch (choosenSide)
				{
				case 1: //default?
					rotationModifier = 0;
					break;
				case 2: //90 degrees
					rotationModifier = 90;
					SpawnRot = FRotator(0.0f, 90.0f, 0.0f);
					break;
				case 3: //180
					rotationModifier = 180;
					SpawnRot = FRotator(0.0f, 180.0f, 0.0f);
					break;
				case 4: //270
					rotationModifier = 270;
					SpawnRot = FRotator(0.0f, 270.0f, 0.0f);
					break;

				default:
					//TODO: Throw error
					UE_LOG(LogTemp, Error, TEXT("No val set"));
					rotationModifier = 0;
					break;
				}

				//physically spawn USFTileVariantDefinitionData->TilePrefab with transform, variant choosen at the 
				//transform of the spawn point in the variant class
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				ASTileVariantEnviornment* SpawnedVariant = GetWorld()->SpawnActor<ASTileVariantEnviornment>(ChoosenVariant, CurrentTile->GetActorLocation(), CurrentTile->GetActorRotation(), SpawnParams);
				SpawnedVariant->SetActorRotation(SpawnRot);

				//an array should be passed up of all the relevant tiles, add them to the VariantEncompassingTiles
				//EncompassingTilesBuild
				//TODO: should also have array doors/walls we want to remove? 
				for (ASTileDoor* doorToDestroy : DoorsArray)
				{
					doorToDestroy->Destroy();
				}

				for (ASTileWall* wallToDestroy : WallArray)
				{
					wallToDestroy->Destroy();
				}

				//if we can, great! mark those tiles with the proper enum
				placed++;
				totalAmount++;
				//TODO: SEt breakpoint to see how logic exits....
				//break;
			}
		}
	}
}

/// <summary>
/// Can place variant at given location
/// </summary>
/// <param name="currentVariant"></param>
/// <param name="offsetTransforms"></param>
/// <param name="CurrentTile"></param>
/// <param name="directionChoosen"></param>
/// <param name="EncompassingTilesBuild"></param>
/// <returns></returns>
bool UTilePathSetupComp::PlugTile(FVariantOffsetTransforms_Rotates transformRotated, USFTileVariantDefinitionData* currentVariant, ASTile* CurrentTile, int& directionChoosen, TArray <ASTile*>& EncompassingTilesBuild, TArray<ASTileDoor*>& DoorsArray, TArray<ASTileWall*>& WallArray)
{
	bool CantPlaceVariant = true;

	//check given orientations the variant can be placed at
	//for each in offset in array

	//as we check through each one, build an array that we can send back if it can be inserted
	//FIntPoint PrevCords = (-1,-1);
	for (FIntPoint GivenOffset : transformRotated.Transforms_flavor)
	{
		//TODO: convert x,z index into FIntPoint Globally
		FIntPoint GridCordToCheck = FIntPoint(CurrentTile->XIndex, CurrentTile->ZIndex);

		if (CurrentTile->TileVariantInUse)
		{
			UE_LOG(LogTemp, Error, TEXT("Current Tile already marked, skipping?"));
			CantPlaceVariant = false;
			break;
		}

		//for each current offset flavor (aka a rotated og offset mapping)
			//offset cords
			FIntPoint OffsetCheck = GridCordToCheck + GivenOffset;

			//check if tile is legit, not null, not starting, not boss room and isn't already marked
			ASTile* OffsetTileToCheck = TileManagerRef->GetGridTilePair(OffsetCheck);
			//no negatives should appear if we null check
			if (OffsetTileToCheck == NULL && OffsetTileToCheck->TileVariantInUse && OffsetTileToCheck->TileStatus == ETileStatus::ETile_BOSSROOM && 
			OffsetTileToCheck->TileStatus == ETileStatus::ETile_NULLROOM && OffsetTileToCheck->TileStatus == ETileStatus::ETile_SECRETROOM && OffsetTileToCheck->TileStatus == ETileStatus::ETile_STARTINGROOM)
			{
				CantPlaceVariant = false;
				break;
			}
		
		//if (PrevCords !=  ( -1, -1))
		//{
		//	//compare prev cords to these cords to determine which wall and door we need to add to array
		//	AddDoorsAndWalls(DoorsArray, WallArray, currentVariant->SidesToCheckOffsets);
		//}
		//PrevCords = OffsetCheck;
	
	}

	if (!CantPlaceVariant)
	{
		//clear arrays before exit
		DoorsArray.Empty();
		WallArray.Empty();
	}
	else {
		AddDoorsAndWalls(DoorsArray, WallArray, currentVariant->SidesToCheckOffsets);
	}

	return CantPlaceVariant;
}

/// <summary>
/// Once we know this variant can be placed, go through each of the doors
/// </summary>
/// <param name="Current"></param>
/// <param name="PrevTile"></param>
/// <param name="CurrentTile"></param>
/// <param name="DoorsArray"></param>
/// <param name="WallArray"></param>
void UTilePathSetupComp::AddDoorsAndWalls(TArray<ASTileDoor*>& DoorsArray, TArray<ASTileWall*>& WallArray, TArray<FIntPointPair> SidesToCheck)
{
	for (FIntPointPair PairToCheck : SidesToCheck)
	{
		//based on the 2 sides (next to each other), determine which is the proper side

		FIntPoint tile1ToCompare = PairToCheck.startCords;
		FIntPoint tile2ToCompare = PairToCheck.endCords;
		ASTile* Tile1 = TileManagerRef->GetGridTilePair(tile1ToCompare);
		ASTile* Tile2 = TileManagerRef->GetGridTilePair(tile2ToCompare);

		if (tile1ToCompare.X == tile2ToCompare.X)
		{
			//if x axis the same, and y axis is one less, then tile1ToCompare's right neighbor aka tile2toCompares's left neighbor
		//same will walls
			if (tile1ToCompare.Y == tile2ToCompare.Y - 1)
			{
				WallArray.Add(Tile1->RightWall);
				DoorsArray.Add(Tile1->RightDoor);
			}
			else if(tile2ToCompare.Y == tile1ToCompare.Y - 1)
			{
				WallArray.Add(Tile1->LeftWall);
				DoorsArray.Add(Tile1->LeftDoor);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("investigate"));
			}
		}
		else if (tile1ToCompare.Y == tile2ToCompare.Y)
		{
			if (tile1ToCompare.X == tile2ToCompare.X - 1)
			{
				WallArray.Add(Tile1->DownWall);
				DoorsArray.Add(Tile1->DownDoor);
			}
			else if (tile2ToCompare.Y == tile1ToCompare.Y - 1)
			{
				WallArray.Add(Tile1->UpWall);
				DoorsArray.Add(Tile1->UpDoor);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("investigate 2"));
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("investigate 3"));
		}

	}
}


