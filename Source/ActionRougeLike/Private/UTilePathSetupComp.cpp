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
	UE_LOG(LogTemp, Log, TEXT("==========================================================="));
	UE_LOG(LogTemp, Log, TEXT("================= PATH GENERATION ========================="));
	
	//once tiles are established, branches and random room additions
	//ChooseStartEndRooms();
	

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

	//GeneratePath();
}


