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

	GridScanForCustomTileSizedVariants();

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
/// Variant Candidate Analysis (if we can place variant and place procedure)
/// </summary>
/// <param name="CurrentTile"></param>
/// <param name="CurrentVariant"></param>
/// <param name="totalAmount"></param>
/// <param name="placed"></param>
void UTileGridBranchComponent::GridScanForCustomTileSizedVariants()
{
	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Starting Custom Sized Tile Variants"));
	//scan through grid where abnormal tiles could potentially be placed
	//only tiles that are off limits would be starting and end tile (TODO: maybe higher tiers of levels could have variants?)
	//TODO: currently this is only starting tile? when do we add all the other tiles? it should be off that right?
	TArray<ASTile*>	ActiveUnusedTiles = TileManagerRef->AllActiveTiles;

	TileManagerRef->TileVariantComponent->SetVariables();

	//TODO: Possible enhancement, maybe we could weight the tiles based on proximity to main path???
	//these candidates will be randomized (shuffle array)

	//each tier of variant types (sizes are grouped into tiers)
	for (int tileVariantTier = 0; tileVariantTier < TileManagerRef->TileVariantComponent->TileVariantTiersLocal.Num(); tileVariantTier++)
	{
		FTileVariantDefinitionRow tier = TileManagerRef->TileVariantComponent->TileVariantTiersLocal[tileVariantTier];

		if (TileManagerRef->DebugPrints)
			UE_LOG(LogTemp, Log, TEXT("Currently on tile tier: %d - number of columns: %d"), tileVariantTier, tier.Columns.Num());

		//each tier of variants has a certain amount (so like 1 of the really big ones and higher number of the smaller sized groups of variants)
		int VariantTierTotalAmountToPlace = FMath::RandRange(tier.Min, tier.Max);
		int VariantsPlaced = 0;

		//each type (so 2x2, 4x4, etc) of variant
		//while we have tiles to place for each tier
		for (int tileVariantType = 0; (tileVariantType < tier.Columns.Num() && VariantsPlaced < VariantTierTotalAmountToPlace); tileVariantType++)
		{
			//we now break things down further, go down each sized variant (so 2x2, 2x1, etc) as we look at each tier (remember each tier has multiple similarly sized variants)
			//check for the highest priority variant (we work down from there)
			USFTileVariantDefinitionData* currentVariant = tier.Columns[tileVariantType];

			//each variant type has a max we can place as well
			int LocalVariantTotalAmount = FMath::RandRange(currentVariant->minorMin, currentVariant->minorMax);
			int localVariantsPlaced = 0;

			//shuffle AllActiveTiles
			ActiveUnusedTiles = ReshuffleTiles(ActiveUnusedTiles);

			currentVariant->SetVariantPaths(); //setup the variant paths from OG offset array for each variant as we need
			UE_LOG(LogTemp, Log, TEXT("Current Variant Size: %d by %d"), currentVariant->Size.X, currentVariant->Size.Y);

			//scan in random order
			//for each randomly choosen candidate (a tile on the grid): 
			for (int tileCount = 0; (tileCount < ActiveUnusedTiles.Num() && VariantsPlaced < VariantTierTotalAmountToPlace && localVariantsPlaced < LocalVariantTotalAmount); tileCount++)
			{
				ASTile* currentTile = ActiveUnusedTiles[tileCount];
				//candidates analysis, pass in current variant, etc
				if (VariantCandidateAnalysis(currentTile, currentVariant))
				{
					localVariantsPlaced++;
					LocalVariantTotalAmount++;
					VariantsPlaced++;
				}
			}
		}
	}

	if (TileManagerRef->DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("Finished Custom Sized Tile Variants"));
}

//template be more efficient
TArray <ASTile*> UTileGridBranchComponent::ReshuffleTiles(TArray <ASTile*> ar)
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
/// Variant Candidate Analysis (if we can place specific variant and placement procedure)
/// </summary>
/// <param name="CurrentTile"></param>
/// <param name="CurrentVariant"></param>
/// <param name="totalAmount"></param>
/// <param name="placed"></param>
bool UTileGridBranchComponent::VariantCandidateAnalysis(ASTile* CurrentTile, USFTileVariantDefinitionData* CurrentVariant)
{
	bool placedStatus = false;
	//current tile is starting point, then we check every transform for variant to see if it fits

	bool isSingleTile = (CurrentVariant->Size.X == 1 && CurrentVariant->Size.Y == 1);

	//check if we can place the variant based on size of variant and availability of tile
	if (!CurrentTile->TileVariantInUse && CurrentTile->IsNotSpecialTile())
	{
		TArray<ASTileDoor*> DoorsArray;
		TArray<ASTileWall*> WallArray;
		TArray<ASTile*> TileArray;
		int counter = 1;

		//for bigger tiles, every direction we can place gets randomly choosen at after for loop
		TArray<int> DirectionsAvailable;
		
		for (FVariantOffsetTransforms_Rotates transform : CurrentVariant->VariantPaths)
		{
			TArray <ASTile*> EncompassingTilesBuild;

			
			//check all offsets based on this main tile starting point and populate corresponding data for setup if it fits!
			if (PlugTile(transform, CurrentVariant, CurrentTile, EncompassingTilesBuild, DoorsArray, WallArray, TileArray))
			{
				DirectionsAvailable.Add(transform.TransformDirection); //make it new int but its not ptr?
			}
			else {
				counter++;
			}
		}

		//choose which direction (if non-empty and non single tile)
		if (!DirectionsAvailable.IsEmpty() || isSingleTile)
		{
			int directionPlacement = TileManagerRef->GameStream.RandRange(0, 3);

			if(!DirectionsAvailable.IsEmpty())
				directionPlacement = DirectionsAvailable[TileManagerRef->GameStream.RandRange(0, DirectionsAvailable.Num() - 1)];

			//which type of abnormal tile variant are we going to place? (like the preset, which preset?)
			int variantIndex = TileManagerRef->GameStream.RandRange(0, CurrentVariant->TileVariantEnviornments.Num() - 1);
			TSubclassOf<ASTileVariantEnviornment> ChoosenVariant = CurrentVariant->TileVariantEnviornments[variantIndex];
			FVector SpawnPos;
			FRotator SpawnRot = FRotator(0.0f, 0.0f, 0.0f);

			//set rotation of TilePrefab
			float rotationModifier = 0;
			
			//start with location of currentTile, rotate based on which side choosen
			switch (directionPlacement)
			{
			case 0: //default?
				rotationModifier = 0;
				break;
			case 1: //90 degrees
				rotationModifier = 90;
				SpawnRot = FRotator(0.0f, 90.0f, 0.0f);
				break;
			case 2: //180
				rotationModifier = 180;
				SpawnRot = FRotator(0.0f, 180.0f, 0.0f);
				break;
			case 3: //270
				rotationModifier = 270;
				SpawnRot = FRotator(0.0f, 270.0f, 0.0f);
				break;

			default:
				UE_LOG(LogTemp, Error, TEXT("Improper direction placement %d"), directionPlacement);
				rotationModifier = 0;
				break;
			}
			UE_LOG(LogTemp, Log, TEXT("Tile to place: %d.%d rotated %f degrees (aka selection %d)"), CurrentVariant->Size.X, CurrentVariant->Size.Y, rotationModifier, directionPlacement);

			//physically spawn USFTileVariantDefinitionData->TilePrefab with transform, variant choosen at the 
			//transform of the spawn point in the variant class
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ASTileVariantEnviornment* SpawnedVariant = GetWorld()->SpawnActor<ASTileVariantEnviornment>(ChoosenVariant, CurrentTile->GetActorLocation(), CurrentTile->GetActorRotation(), SpawnParams);
			SpawnedVariant->SetActorRotation(SpawnRot);
			FString VariantTileName = "VariantTileMap_" + FString::FromInt(CurrentVariant->Size.X) + "_" + FString::FromInt(CurrentVariant->Size.Y);
			SpawnedVariant->SetActorLabel(VariantTileName);
#if WITH_EDITOR
			SpawnedVariant->SetFolderPath(TileManagerRef->VariantTileMapSubFolderName);

#endif

			//an array should be passed up of all the relevant tiles, add them to the VariantEncompassingTiles
			//EncompassingTilesBuild
			//TODO: should also have array doors/walls we want to remove? 
			for (ASTileDoor* doorToDestroy : DoorsArray)
			{
				doorToDestroy->DoorActive = false;
			}

			for (ASTileWall* wallToDestroy : WallArray)
			{
				wallToDestroy->Destroy();
			}

			//mark base tiles to be non usable 
			for (ASTile* tileToMark : TileArray)
			{
				tileToMark->TileVariantInUse = true;
			}


			//if we can, great!
			placedStatus = true;
		}
	}

	return placedStatus;
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
bool UTileGridBranchComponent::PlugTile(FVariantOffsetTransforms_Rotates transformRotated, USFTileVariantDefinitionData* currentVariant, ASTile* CurrentTile, TArray <ASTile*>& EncompassingTilesBuild, TArray<ASTileDoor*>& DoorsArray, TArray<ASTileWall*>& WallArray, TArray<ASTile*>& TileArray)
{
	bool CantPlaceVariant = true;

	//check given orientations the variant can be placed at
	//for each in offset in array

	//as we check through each one, build an array that we can send back if it can be inserted
	//FIntPoint PrevCords = (-1,-1);
	for (FIntPoint GivenOffset : transformRotated.Transforms_flavor) //each index of variant paths is passed in via transformRotated, and then each of those indexs has the transform flavors array to index through
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
		UE_LOG(LogTemp, Log, TEXT("GridCordToCheck: %d.%d - offset to apply: %d.%d - Therefore Checking tile: %d,%d"), GridCordToCheck.X, GridCordToCheck.Y, GivenOffset.X, GivenOffset.Y, OffsetCheck.X, OffsetCheck.Y);

		//check if tile is legit, not null, not starting, not boss room and isn't already marked
		ASTile* OffsetTileToCheck = TileManagerRef->GetGridTilePair(OffsetCheck);
		if(OffsetTileToCheck != NULL)
			UE_LOG(LogTemp, Log, TEXT("Retrieved tile: %d,%d"), OffsetTileToCheck->XIndex, OffsetTileToCheck->ZIndex);
		//no negatives should appear if we null check
		if (OffsetTileToCheck == NULL || OffsetTileToCheck->TileVariantInUse || OffsetTileToCheck->TileStatus == ETileStatus::ETile_BOSSROOM ||
			OffsetTileToCheck->TileStatus == ETileStatus::ETile_NULLROOM || OffsetTileToCheck->TileStatus == ETileStatus::ETile_SECRETROOM || OffsetTileToCheck->TileStatus == ETileStatus::ETile_STARTINGROOM)
		{
			CantPlaceVariant = false;
			break;
		}
		TileArray.Add(OffsetTileToCheck);

		//if (PrevCords !=  ( -1, -1))
		//{
		//	//compare prev cords to these cords to determine which wall and door we need to add to array
		//	AddDoorsAndWalls(DoorsArray, WallArray, currentVariant->SidesToCheckOffsets);
		//}
		//PrevCords = OffsetCheck;

	}

	if (!CantPlaceVariant)
	{
		//clear arrays before exit (prep vars)
		DoorsArray.Empty();
		WallArray.Empty();
		TileArray.Empty();
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
void UTileGridBranchComponent::AddDoorsAndWalls(TArray<ASTileDoor*>& DoorsArray, TArray<ASTileWall*>& WallArray, TArray<FIntPointPair> SidesToCheck)
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
			else if (tile2ToCompare.Y == tile1ToCompare.Y - 1)
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
		if (selected.tile->UpNeighbor == NULL) //if no neighbor, we spawn a tile and add the 3 walls + door
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X, selected.tile->GetActorLocation().Y - (TileManagerRef->StartingGridTile->TileLength), selected.tile->GetActorLocation().Z); //+ 240
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
		TileManagerRef->SecretRoom->SpawnDoorConnector(ETileSide::ETile_Down, TileManagerRef->ChoosenDoorwayAsset, TileManagerRef->DoorSubFolderName, TileManagerRef->AllSpawnedWalls);

		break;
	case 2:
		//down

		//TODO: may need to fix rotation? 
		if (selected.tile->DownNeighbor == NULL)
		{
			SpawnPos = FVector(selected.tile->GetActorLocation().X, selected.tile->GetActorLocation().Y + (TileManagerRef->StartingGridTile->TileLength), selected.tile->GetActorLocation().Z);
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
		TileManagerRef->SecretRoom->SpawnDoorConnector(ETileSide::ETile_Up, TileManagerRef->ChoosenDoorwayAsset, TileManagerRef->DoorSubFolderName, TileManagerRef->AllSpawnedWalls);
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
		TileManagerRef->SecretRoom->SpawnDoorConnector(ETileSide::ETile_Right, TileManagerRef->ChoosenDoorwayAsset, TileManagerRef->DoorSubFolderName, TileManagerRef->AllSpawnedWalls);
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
		TileManagerRef->SecretRoom->SpawnDoorConnector(ETileSide::ETile_Left, TileManagerRef->ChoosenDoorwayAsset, TileManagerRef->DoorSubFolderName, TileManagerRef->AllSpawnedWalls);
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
	TileManagerRef->SecretRoom->ActivateWalls(TileManagerRef->ChoosenWallAsset, TileManagerRef->WallsSubFolderName, TileManagerRef->AllSpawnedWalls);
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
	TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAssetRef = TileManagerRef->ChoosenDoorwayAsset;
	FName WallsSubFolderNameRef = TileManagerRef->WallsSubFolderName;
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
				CurrentTile->ConnectUpDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
				return;
			}
			break;
			//
		case 2:
			if (CurrentTile->HasConnectedDownNeighbor())
			{
				CurrentTile->ConnectDownDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
				return;
			}
			break;
		case 3:
			if (CurrentTile->HasConnectedLeftNeighbor())
			{
				CurrentTile->ConnectLeftDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
				return;
			}
			break;
		case 4:
			if (CurrentTile->HasConnectedRightNeighbor())
			{
				CurrentTile->ConnectRightDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
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

	if(door == NULL)
		UE_LOG(LogTemp, Log, TEXT("Array isnt empty? why this run more than once on this guy?"));

	door->DoorActive = true;
	tile->RemoveCurrentWall(SideToSpawnDoor);
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
/// (Passed in via prevDirection where 1 = Up, 2 = down, 3 = left, 4 = right)
/// </summary>
/// <param name="TileToAdd"></param>
/// <param name="prevDirection"></param>
void UTileGridBranchComponent::ConnectDoorBranch(ASTile* TileToAdd, int prevDirection)
{
	TSubclassOf<ASTileDoorWallConnection> ChoosenDoorwayAssetRef = TileManagerRef->ChoosenDoorwayAsset;
	FName WallsSubFolderNameRef = TileManagerRef->WallsSubFolderName;
	switch (prevDirection) {
	case 1: //prev was up
		//prev tile was up direction to get here, therefore this tile's down neighbor was the up neighbor of the prev tile
		TileToAdd->ConnectDownDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		break;
	case 2: //prev was down
		TileToAdd->ConnectUpDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		break;
	case 3: //prev was left
		TileToAdd->ConnectRightDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		break;
	case 4: //prev was right
		TileToAdd->ConnectLeftDoor(ChoosenDoorwayAssetRef, WallsSubFolderNameRef, TileManagerRef->AllSpawnedWalls);
		break;
	}
}