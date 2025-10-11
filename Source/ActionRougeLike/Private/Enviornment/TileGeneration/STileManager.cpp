// Fill out your copyright notice in the Description page of Project Settings.


#include "Enviornment/TileGeneration/STileManager.h"
#include "SLocalLevel.h"
#include "UTilePathSetupComp.h"
#include "UTileGridBranchComponent.h"
#include "ULevelAssetSetupComponent.h"
#include <string>
#include <Math/UnrealMathUtility.h>
#include <Kismet/KismetMathLibrary.h>
#include "SFTileVariantDefinitionData.h"

// Sets default values
ASTileManager::ASTileManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TilesRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TilesRoot"));
	RootComponent = TilesRoot;

	TilePathComponent = CreateDefaultSubobject<UTilePathSetupComp>(TEXT("TilePathSetupComponent"));
	//bind to generation event
	TilePathComponent->OnPathGeneratedEvent.AddDynamic(this, &ASTileManager::OnTilePathGeneration);

	GridBranchSetupComponent = CreateDefaultSubobject<UTileGridBranchComponent>(TEXT("BranchSetupComponent"));
	GridBranchSetupComponent->OnGridAdditionalSetupCompletedEvent.AddDynamic(this, &ASTileManager::OnBranchFillGeneration);

	LevelAssetSetupComponent = CreateDefaultSubobject<ULevelAssetSetupComponent>(TEXT("LevelAssetSetupComponent"));
	LevelAssetSetupComponent->TileManagerRef = this;

	//TileVariantComponent = CreateDefaultSubobject<UTileVariantComponent>(TEXT("TileVariantComponent"));
	

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

	//TO DO: Make a set var function?
	TileVariantComponent = FindComponentByClass<UTileVariantComponent>();

	if (DebugPrints) {
		UE_LOG(LogTemp, Log, TEXT("==========================================================="));
		UE_LOG(LogTemp, Log, TEXT("================= TILE GENERATION ========================="));
		UE_LOG(LogTemp, Log, TEXT("==========================================================="));
	}
	SeedSetup();

	SetVariables();

	//create and link tiles into grid
	//this includes establishment of doors if we need them
	Create2DTileArray();

	TilePathComponent->TilePathGeneration();

}

void ASTileManager::SetVariables()
{
//tile variants priorities are hardcoded
	/*TileVariants = {
		FTileVariantDefinition(ETileSizeVariant::ET1x1, FIntPoint(1,1), 1, ),
		FTileVariantDefinition(ETileSizeVariant::ET2x1, FIntPoint(2,1), 4),
		FTileVariantDefinition(ETileSizeVariant::ET2x2, FIntPoint(2,2), 5),
		FTileVariantDefinition(ETileSizeVariant::ET3x1, FIntPoint(3,1), 5),
		FTileVariantDefinition(ETileSizeVariant::ET3x2, FIntPoint(3,2), 7),
		FTileVariantDefinition(ETileSizeVariant::ET4x2, FIntPoint(4,2), 8),
		FTileVariantDefinition(ETileSizeVariant::ET4x3, FIntPoint(4,3), 8),
		FTileVariantDefinition(ETileSizeVariant::ET4x4, FIntPoint(4,4), 8)
	};*/

	//shorthand sort (based on priority
	/*TileVariants.Sort([](const FTileVariantDefinition& A, const FTileVariantDefinition& B)
		{
			return A.Priority < B.Priority;
		}
	);*/


	//TileVariantTiers = TileVariantComponent->TileVariantTiersLocal;

}

/// <summary>
/// Finished Path Generation
/// 
/// Next is Branches and Random rooms
/// </summary>
void ASTileManager::OnTilePathGeneration()
{
	//finished the main path creation, now do branch, random rooms and secret room setup

	GridBranchSetupComponent->GameMapAdditionalSetup();
}

void ASTileManager::OnBranchFillGeneration()
{
	RemoveUnusedOuters();

	//Level asset spawn can now begin
	LevelAssetSetupComponent->PopulateGrid();
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

	int floatingWallBuffer = ChoosenWallAsset->GetDefaultObject<ASTileWall>()->WallsBuffer;
	int distanceToNextTile = floatingWallBuffer * 2;

	int tileLength = TileBase->GetDefaultObject<ASTile>()->TileLength + distanceToNextTile;
	
	for (int32 XIndex = 0; XIndex < LevelWidth; XIndex++)
	{
		//for each row, make each column
		FMultiTileStruct* Col = new FMultiTileStruct();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		//Populate TileColumn array with Tiles for Height size
		for (int32 ZIndex = 0; ZIndex < LevelHeight; ZIndex++)
		{
			//spawn in a Tile
			FString TileName = "Tile_Row" + FString::FromInt(XIndex) + "_Col" + FString::FromInt(ZIndex);
			FVector ORLocal((this->GetActorLocation().X + (tileLength * XIndex)), (this->GetActorLocation().Y + (tileLength * ZIndex)), this->GetActorLocation().Z);
			FVector TileSpawnLocation((this->GetActorLocation().X + (tileLength * XIndex)), (this->GetActorLocation().Y + (tileLength * ZIndex)), this->GetActorLocation().Z);
			
			ASTile* T = GetWorld()->SpawnActor<ASTile>(TileBase, TileSpawnLocation, this->GetActorRotation(), SpawnParams);
			UE_LOG(LogTemp, Log, TEXT("TileName %s, OG: %s, New: %s"), *TileName, *ORLocal.ToString(), *TileSpawnLocation.ToString());

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

			////if top row or bottom row add walls
			//if (ZIndex == 0) //nothing to left so add wall
			//{

			//}
			//else if (ZIndex == LevelHeight - 1)
			//{
			//	//nothing to right so add wall
			//}
			//else if (XIndex == 0)
			//{
			//	//nothing below so add wall
			//}
			//else if (XIndex == LevelWidth - 1)
			//{
			//	//nothing above so add wall
			//}

		}

		Grid2DArray.Add(Col);
	}
	totalGridTilesAvailable = (LevelHeight * LevelWidth) * gridDensity;

	//add walls to perimeter


	if (DebugPrints)
		UE_LOG(LogTemp, Log, TEXT("=================== 2D array CREATED! =============================="));
}

/// <summary>
/// Add walls to perimeter
/// Always runs regardless of if inner walls are implemented
/// </summary>
void ASTileManager::AddWallToPerimeter(ETileSide side, ASTile* ThisTile)
{
	FVector WallLocation;
	FTransform WallSpawnTrans;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	switch (side)
	{
	case ETileSide::ETile_Up:
		WallLocation = ThisTile->SM_UpWallSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
		WallSpawnTrans = FTransform(ThisTile->SM_UpWallSpawnPoint.GetRotation(), WallLocation);
		ThisTile->UpWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		ThisTile->UpWall->InnerTile = ThisTile;
		ThisTile->UpWall->RemoveOuter();
		AllSpawnedWalls.Add(ThisTile->UpWall);

		ThisTile->UpWall->SetOwner(ThisTile);
#if WITH_EDITOR
		ThisTile->UpWall->SetFolderPath(WallsSubFolderName);
#endif
		break;
	case ETileSide::ETile_Down:
		WallLocation = ThisTile->SM_DownWallSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
		WallSpawnTrans = FTransform(ThisTile->SM_DownWallSpawnPoint.GetRotation(), WallLocation);
		ThisTile->DownWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		ThisTile->DownWall->InnerTile = ThisTile;
		ThisTile->DownWall->RemoveOuter();
		AllSpawnedWalls.Add(ThisTile->DownWall);

		ThisTile->DownWall->SetOwner(ThisTile);
#if WITH_EDITOR
		ThisTile->DownWall->SetFolderPath(WallsSubFolderName);
#endif
		break;
	case ETileSide::ETile_Left:
		WallLocation = ThisTile->SM_LeftWallSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
		WallSpawnTrans = FTransform(ThisTile->SM_LeftWallSpawnPoint.GetRotation(), WallLocation);
		ThisTile->LeftWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		ThisTile->LeftWall->InnerTile = ThisTile;
		ThisTile->LeftWall->RemoveOuter();
		AllSpawnedWalls.Add(ThisTile->LeftWall);

		ThisTile->LeftWall->SetOwner(ThisTile);
#if WITH_EDITOR
		ThisTile->LeftWall->SetFolderPath(WallsSubFolderName);
#endif
		break;
	case ETileSide::ETile_Right:
		WallLocation = ThisTile->SM_RightWallSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
		WallSpawnTrans = FTransform(ThisTile->SM_RightWallSpawnPoint.GetRotation(), WallLocation);
		ThisTile->RightWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		ThisTile->RightWall->InnerTile = ThisTile;
		ThisTile->RightWall->RemoveOuter();
		AllSpawnedWalls.Add(ThisTile->RightWall);

		ThisTile->RightWall->SetOwner(ThisTile);
#if WITH_EDITOR
		ThisTile->RightWall->SetFolderPath(WallsSubFolderName);
#endif
		break;
	default:
		WallLocation = ThisTile->SM_RightWallSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
		WallSpawnTrans = FTransform(ThisTile->SM_RightWallSpawnPoint.GetRotation(), WallLocation);
		ThisTile->RightWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
		ThisTile->RightWall->InnerTile = ThisTile;
		ThisTile->RightWall->RemoveOuter();
		AllSpawnedWalls.Add(ThisTile->RightWall);

		ThisTile->RightWall->SetOwner(ThisTile);
#if WITH_EDITOR
		ThisTile->RightWall->SetFolderPath(WallsSubFolderName);
#endif
		UE_LOG(LogTemp, Error, TEXT("Default param for RemovePlaceholderWall"));
		break;

	}
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

		CurrentTile->ConnectUpDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
		doorTransform = CurrentTile->UpDoor->GetTransform();
	}
	else if ((CurrentTile->DownNeighbor && CurrentTile->DownNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->DownNeighbor->XIndex, CurrentTile->DownNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->DownNeighbor);
		CurrentTile->ConnectDownDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
		doorTransform = CurrentTile->DownDoor->GetTransform();
	}
	else if ((CurrentTile->RightNeighbor && CurrentTile->RightNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->RightNeighbor->XIndex, CurrentTile->RightNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->RightNeighbor);
		CurrentTile->ConnectRightDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
		doorTransform = CurrentTile->RightDoor->GetTransform();
	}
	else if ((CurrentTile->LeftNeighbor && CurrentTile->LeftNeighbor->IsBossTile()))
	{
		UE_LOG(LogTemp, Log, TEXT("Found Boss Room! at %d,%d"), CurrentTile->LeftNeighbor->XIndex, CurrentTile->LeftNeighbor->ZIndex);
		CurrentTile->CheckForPath = true;
		AddTileToPath(CurrentTile);
		AddTileToPath(CurrentTile->LeftNeighbor);
		CurrentTile->ConnectLeftDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
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
					CurrentTile->ActivateUpDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
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
					CurrentTile->ActivateDownDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
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
					CurrentTile->ActivateLeftDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
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
					CurrentTile->ActivateRightDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
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
		TileToAdd->ConnectDownDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
		break;
	case 2: //prev was down
		TileToAdd->ConnectUpDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
	
		break;
	case 3: //prev was left
		TileToAdd->ConnectRightDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
		break;
	case 4: //prev was right
		TileToAdd->ConnectLeftDoor(ChoosenDoorwayAsset, WallsSubFolderName, AllSpawnedWalls);
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
			const FTransform SpawmTrans = FTransform(ThisTile->UpDoorSpawnPoint.GetRotation(), UpDoorSpawnLocation);
			//UE_LOG(LogTemp, Log, TEXT("UpDoorSpawnLocation: %s"), *UpDoorSpawnLocation.ToString());
			ThisTile->UpDoor = GetWorld()->SpawnActor<ASTileDoor>(TileDoor, SpawmTrans, SpawnParams);
			DoorArray.Add(ThisTile->UpDoor);
			ThisTile->UpDoor->SetActorLabel(TileUpDoorName);
			ThisTile->UpDoor->SetOwner(ThisTile);
#if WITH_EDITOR
			ThisTile->UpDoor->SetFolderPath(DoorSubFolderName);
#endif
			UpNeighbor->DownDoor = ThisTile->UpDoor;

			//set up wall
			
			//spawn 1 ASTileWall object & set it to be this tiles upwall
			const FVector UpWallLocation = ThisTile->SM_UpWallSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
			const FTransform WallSpawnTrans = FTransform(ThisTile->SM_UpWallSpawnPoint.GetRotation(), UpWallLocation);
			ThisTile->UpWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);
			//UE_LOG(LogTemp, Log, TEXT("UpWallSpawnLocation: %s"), *UpWallLocation.ToString());
			//set it to be UpNeighbor's down wall
			UpNeighbor->DownWall = ThisTile->UpWall;
			//on the ASTileWall, set Inner to be this tile
			ThisTile->UpWall->InnerTile = ThisTile;
			//set outer to ThisTiles->upniehgbor
			ThisTile->UpWall->OuterTile = UpNeighbor;
			ThisTile->UpWall->SetOwner(ThisTile);
			AllSpawnedWalls.Add(ThisTile->UpWall);
#if WITH_EDITOR
			ThisTile->UpWall->SetFolderPath(WallsSubFolderName);
#endif

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


			//set up wall

			//spawn 1 ASTileWall object & set it to be this tiles upwall
			const FVector LeftWallLocation = ThisTile->SM_LeftWallSpawnPoint.GetLocation() + ThisTile->GetActorLocation();
			const FTransform WallSpawnTrans = FTransform(ThisTile->SM_LeftWallSpawnPoint.GetRotation(), LeftWallLocation);
			ThisTile->LeftWall = GetWorld()->SpawnActor<ASTileWall>(ChoosenWallAsset, WallSpawnTrans, SpawnParams);

			//set it to be UpNeighbor's down wall
			LeftNeighbor->RightWall = ThisTile->LeftWall;
			//on the ASTileWall, set Inner to be this tile
			ThisTile->LeftWall->InnerTile = ThisTile;
			//set outer to ThisTiles->upniehgbor
			ThisTile->LeftWall->OuterTile = LeftNeighbor;
			ThisTile->LeftWall->SetOwner(ThisTile);
			AllSpawnedWalls.Add(ThisTile->LeftWall);
#if WITH_EDITOR
			ThisTile->LeftWall->SetFolderPath(WallsSubFolderName);
#endif
		}
	}

	UE_LOG(LogTemp, Log, TEXT("hi"));
	//add outer walls
	if (ThisTile->ZIndex == 0) //no up or down neighbor
	{
		UE_LOG(LogTemp, Log, TEXT("hi"));
		AddWallToPerimeter(ETileSide::ETile_Up, ThisTile);
	}
	if (ThisTile->ZIndex == LevelWidth - 1)
	{
		UE_LOG(LogTemp, Log, TEXT("hi"));
		AddWallToPerimeter(ETileSide::ETile_Down, ThisTile);
	}
	if (ThisTile->XIndex == 0) //no right nor left neighbor
	{
		UE_LOG(LogTemp, Log, TEXT("hi"));
		AddWallToPerimeter(ETileSide::ETile_Left, ThisTile);
	}
	if (ThisTile->XIndex == LevelHeight - 1)
	{
		UE_LOG(LogTemp, Log, TEXT("hi"));
		AddWallToPerimeter(ETileSide::ETile_Right, ThisTile);
	}
}

// Called every frame
void ASTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//For Spawning doors to attach to tiles
void ASTileManager::SpawnDoor(ASTile* tile, ETileSide SideToSpawnDoor, FString NameOfTileToConnect)
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

	
	GridBranchSetupComponent->SetupDoor(tile, SideToSpawnDoor, NameOfTileToConnect, door);
}

/// <summary>
/// Run through all walls, and remove their outers if marked
/// </summary>
void ASTileManager::RemoveUnusedOuters()
{
	for (ASTileWall* SpawnedWall : AllSpawnedWalls)
	{
		if (SpawnedWall != NULL)
		{
			if (SpawnedWall->OuterWallObject != NULL && SpawnedWall->WillRemoveOuter)
			{
				SpawnedWall->OuterWallObject->DestroyComponent();
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("null val in wall array?"));
		}
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
/// Get Specific Tile in Grid
/// </summary>
/// <param name="X"></param>
/// <param name="Y"></param>
/// <returns></returns>
ASTile* ASTileManager::GetGridTile(int32 Y, int32 X)
{
	return Grid2DArray[Y]->TileColumn[X];
}

/// <summary>
/// Get Specific Tile in Grid (overload)
/// TODO: Use the null logic for other overloads
/// </summary>
/// <param name="TileCords"></param>
/// <returns></returns>
ASTile* ASTileManager::GetGridTilePair(FIntPoint TileCords)
{
	if (TileCords.X < LevelWidth && TileCords.X >= 0 && TileCords.Y < LevelHeight && TileCords.Y >= 0)
	{
		return Grid2DArray[TileCords.X]->TileColumn[TileCords.Y]; //had these swapped?
	}
	else {
		return NULL;
	}
	
}

/// <summary>
///  ensures that as the path expands, the branch factor scales to avoid excessive isolation.
/// </summary>
/// <returns></returns>
float ASTileManager::BranchDensityFactor_DynamicMainPathLength()
{
	if (LevelWidth <= 0 && LevelHeight <= 0)
		UE_LOG(LogTemp, Error, TEXT("Error: No valid grid width and height to determine branch density"));

	//Keep this for now, may be useful: how much density is left after path?
	//float remainingGridDensity = 1.0f - ((float)LevelPath.Num() / ((float)LevelWidth * (float)LevelHeight));
	//UE_LOG(LogTemp, Log, TEXT("Choice 1.5: %f"), remainingGridDensity);
	float branchDensityFactor = FMath::Clamp((LevelPath.Num() / (float)LevelWidth), 0.05f, 0.2f);

	return branchDensityFactor;
}

