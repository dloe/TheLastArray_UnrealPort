// Copyright (c) 2026 Dylan.
// Personal Game Project.
//
// This code is provided as-is for development and experimentation.
// Unauthorized use, distribution, or modification is not permitted.

#include "Enviornment/ULevelAssetSetupComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enviornment/UTileGridBranchComponent.h"
#include "Engine/AssetManager.h"
#include "Actions/SActionComponent.h"
#include "ScriptableObjects/SMonsterData.h"
#include "Enviornment/PickupAssetData.h"
#include "AI/SFEnemyDataDefinition.h"
#include "UObject/UObjectGlobals.h"
#include "Delegates/Delegate.h"
#include "Objectives/SEliminationObjective.h"
#include "Gamemodes/SMainGameMode.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"


//Note: This component is purely for placement and choosing of level assets (IE Items and objectives?)
// and enemies



/// -------------   Main Idea for component:  -------------
/// Will for now make the spawned items populate an array of pickups. Prototype via blockout tiles to be placed and run core logic,
/// will have the preplaced 'markers'.
/// 1. Make variant block-outs for example lvl. Include the preplacements of items and enemies
/// 2. Perlin noise system and how to determine which places to use.
/// 3. weight table for which items to spawn
/// 4. Spawn item function

// Sets default values for this component's properties
ULevelAssetSetupComponent::ULevelAssetSetupComponent()
{
	// ...
}


// Called when the game starts
void ULevelAssetSetupComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...
}

/// <summary>
/// Since we cant run much in BeginPlay due to TileManager setting everything up, that setup is now run here
/// </summary>
void ULevelAssetSetupComponent::SetupLevelAssetComponent()
{
	SpawnTileEnvRef = TileManagerRef->GetPlayerSpawnPresentTile();

	seedOffset_Items.X = LocalLevel->GameStream.RandRange(-500, 500);
	seedOffset_Items.Y = LocalLevel->GameStream.RandRange(-500, 500);

	seedOffset_Enemies.X = LocalLevel->GameStream.RandRange(-500, 500);
	seedOffset_Items.Y = LocalLevel->GameStream.RandRange(-500, 500);

	GridBranchCompRef = TileManagerRef->GetGridBranchComp();
	//go through all placed tiles (secret room should be separate)
	SpawnedVariantsRef = GridBranchCompRef->GetSpawnedVariantTiles();

	switch (LocalLevel->CurrentLevelTier)
	{
	case ELevelTier::ELevel_1:
		LocalEnemyInfoData = EnemyData->TieredData[0];
		LocalItemInfoData = ItemData->ItemPickupTable[0];
		LocalObjectiveInfoData = ObjectiveData->ObjectivesTable[0];
		break;
	case ELevelTier::ELevel_2:
		LocalEnemyInfoData = EnemyData->TieredData[1];
		LocalItemInfoData = ItemData->ItemPickupTable[1];
		LocalObjectiveInfoData = ObjectiveData->ObjectivesTable[1];
		break;
	case ELevelTier::ELevel_3:
		LocalEnemyInfoData = EnemyData->TieredData[2];
		LocalItemInfoData = ItemData->ItemPickupTable[2];
		LocalObjectiveInfoData = ObjectiveData->ObjectivesTable[2];
		break;
	case ELevelTier::ELevel_4:
		LocalEnemyInfoData = EnemyData->TieredData[3];
		LocalItemInfoData = ItemData->ItemPickupTable[3];
		LocalObjectiveInfoData = ObjectiveData->ObjectivesTable[3];
		break;
	case ELevelTier::ELevel_Null:
	default:
		LocalEnemyInfoData = EnemyData->TieredData[1];
		LocalItemInfoData = ItemData->ItemPickupTable[1];
		LocalObjectiveInfoData = ObjectiveData->ObjectivesTable[1];
		break;
	}

	TArray<FEnemySpawnInfo*> EnemySpawnItems;
	//the string being pass in is for debugging purposes in case it breaks or fails to cast
	EnemyData->MonsterTable->GetAllRows("", EnemySpawnItems);

	//pull Ids from monster table to construct which choices are available for spawning
	for (FEnemySpawnInfo* DataRowEnemy : EnemySpawnItems)
	{
		//if IDs match, add to our list
		if (DataRowEnemy && LocalEnemyInfoData.EnemyPoolID.Contains(DataRowEnemy->MonsterId))
		{
			CurrentLevelEnemyList.Add(DataRowEnemy);
		}

	}

	//set up the asset count array
	TArray<FItemPickupAsset*> ItemSpawnItems;
	//the string being pass in is for debugging purposes in case it breaks or fails to cast
	ItemData->ItemTable->GetAllRows("", ItemSpawnItems);

	//build out an array of FLocalLevelItemSpawnTiers objects (each on is a item tier specific for this level with the specific configs)
    //trying a new way
	CurrentLevelItemTierList = {
		//for some reason the new keyword has issues with the FLocalLevelItemSpawnTiers being USTRUCT
		//removed it for now cause it doesn't seem to make a difference
		FLocalLevelItemSpawnTiers(ESpawnTiers::ECommon),      //0
		FLocalLevelItemSpawnTiers(ESpawnTiers::EUncommon),    //1
		FLocalLevelItemSpawnTiers(ESpawnTiers::ERare),        //2
		FLocalLevelItemSpawnTiers(ESpawnTiers::EEpic),        //3
		FLocalLevelItemSpawnTiers(ESpawnTiers::ELegendary),   //4
		FLocalLevelItemSpawnTiers(ESpawnTiers::EObjective)    //5 (but not gunna populate until objectives are choosen and set
	};

	//pull Ids from monster table to construct which choices are available for spawning
	for (FItemPickupAsset* DataRowItem : ItemSpawnItems)
	{
		//items that match the TierWeightsPerLevel.ItemPoolId and ItemTable should be put into the array
		//all tiers (since this is broken up different, will need to be slightly more complicated to build out 
		//our structure for easy access
		//idea here is that we want to have a tiered structure of FItemPickupAsset's for easy access per level per tier
		
		if (DataRowItem)
		{
			switch (DataRowItem->ItemTier)
			{
				case ESpawnTiers::ECommon:
				//do i need a check for ID here?
					CurrentLevelItemTierList[0].ItemPoolAsset.Add(DataRowItem);
					break;
				case ESpawnTiers::EUncommon:
					CurrentLevelItemTierList[1].ItemPoolAsset.Add(DataRowItem);
					break;
				case ESpawnTiers::ERare:
					CurrentLevelItemTierList[2].ItemPoolAsset.Add(DataRowItem);
					break;
				case ESpawnTiers::EEpic:
					CurrentLevelItemTierList[3].ItemPoolAsset.Add(DataRowItem);
					break;
				case ESpawnTiers::ELegendary:
					CurrentLevelItemTierList[4].ItemPoolAsset.Add(DataRowItem);
					break;
				case ESpawnTiers::EObjective:
				default:
					break;
			}
		}
	}

	//set up texture sizing for perlin noise map
	for (int y = 0; y < LocalLevel->GameMapTextureSize; y++)
	{
		for (int x = 0; x < LocalLevel->GameMapTextureSize; x++)
		{
			//see comments below for more detail
			float cordX = (x + seedOffset_Items.X) / float(LocalLevel->GameMapTextureSize - 1);
			float cordY = (y + seedOffset_Items.Y) / float(LocalLevel->GameMapTextureSize - 1);
			float NoiseCheck = FMath::PerlinNoise2D(FVector2D(cordX, cordY) * LocalLevel->PerlinScaleFreq);
			MinNoiseItems = FMath::Min(MinNoiseItems, NoiseCheck);
			MaxNoiseItems = FMath::Max(MaxNoiseItems, NoiseCheck);

			float cordX2 = (x + seedOffset_Enemies.X) / float(LocalLevel->GameMapTextureSize - 1);
			float cordY2 = (y + seedOffset_Enemies.Y) / float(LocalLevel->GameMapTextureSize - 1);
			NoiseCheck = FMath::PerlinNoise2D(FVector2D(cordX2, cordY2) * LocalLevel->PerlinScaleFreq);
			MinNoiseEnemies = FMath::Min(MinNoiseEnemies, NoiseCheck);
			MaxNoiseEnemies = FMath::Max(MaxNoiseEnemies, NoiseCheck);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Items - min: %f, max: %f"), MinNoiseItems, MaxNoiseItems);
	UE_LOG(LogTemp, Log, TEXT("Enemies - min: %f, max: %f"), MinNoiseEnemies, MaxNoiseEnemies);

	//debug perlin noise texture for easy visualization
	if (ActivateDebugFloorPerlinNoise)
	{
		SetUpDebugPerlinNoise();
	}
}

void ULevelAssetSetupComponent::ActivateSecretRoom()
{
	//get placed secret tile ASTileVariantEnviornment
	ASTileVariantEnviornment* SecretRoom = GridBranchCompRef->GetSecretRoomEnv();

	//a one time add TilePlaced cleanup to OnCleanupDelegate
	OnCleanupPickups.AddUObject(SecretRoom, &ASTileVariantEnviornment::HandleMarkerCleanup);
	int currentAssetPlacedCount = 0;

	//check each pre-placed pickup
	for (UStaticMeshComponent* PossiblePickup : SecretRoom->PickupPlacements)
	{
		check(PossiblePickup); //trying this check 
		const FVector relativeLocation = PossiblePickup->GetRelativeLocation();
		//check noise 
		FVector2D inputConvertionSeedOffset = FVector2D(relativeLocation.X + seedOffset_Items.X, relativeLocation.Y + seedOffset_Items.Y);
		float noiseMeasurement = GetNoiseVec(inputConvertionSeedOffset, MinNoiseItems, MaxNoiseItems);

		//threshold check TODO: This will be assigned from 
		float itemThreshold = LocalLevel->GetLocalPickupSpawnLevelThreshold();
		//UE_LOG(LogTemp, Log, TEXT("Comparing noise val: %f to threshold: %f"), noiseMeasurement, itemThreshold);
		//if meeds threshold, spawn item function for weight lookup and spawn procedure
		if (noiseMeasurement <= itemThreshold * 2) //slight bump for secret room (for now doubling) TODO: Maybe tie to separate stat per level?
		{
			//can spawn!
			PlaceItemPickup(PossiblePickup, SecretRoom, PickupsPlaced);

			//increment counter
			PickupsPlaced++;
			currentAssetPlacedCount++;
			UE_LOG(LogTemp, Log, TEXT("Item %d placed"), PickupsPlaced);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Secret Tile complete %s, local total: %d"), *SecretRoom->GetActorLabel(), currentAssetPlacedCount);
}

/// <summary>
/// All objectives are children of parent objective class
/// This must run before we find a places for items btw
/// Objective data object will hold array of objectives per level 
/// </summary>
void ULevelAssetSetupComponent::ActivateObjectives()
{
	UE_LOG(LogTemp, Log, TEXT("--- Choosing objectives ---"));
	//if lvl 4, only pick boss objective (should be set up in data beforehand anyway)

	int ChoosenObjectiveIndex = (OverrideObjectiveChoice == -1) ? 
	LocalLevel->GameStream.RandRange(0, LocalObjectiveInfoData.PossibleObjectives.Num()) : OverrideObjectiveChoice;

	//else we choose randomly from array
	TSubclassOf<USBaseObjective> ChoosenObjective = LocalObjectiveInfoData.PossibleObjectives[ChoosenObjectiveIndex];

	USBaseObjective* CurrentObjective = NewObject<USBaseObjective>(this, ChoosenObjective);
	CurrentObjective->LevelTier = LocalObjectiveInfoData.LevelTier;
	CurrentObjective->Initialize(this);

	//objective setup

	//check if elimination obj so we can set the event delegate
	if (CurrentObjective->IsA<USEliminationObjective>())
	{
		//have the completion of each objective (kill special person, interact with object, kill x amount of enemies, etc)
		// will broadcast an event
		USEliminationObjective* EliminationObj = Cast<USEliminationObjective>(CurrentObjective);
		OnEnemySpawnCompletedEvent.AddDynamic(EliminationObj, &USEliminationObjective::OnEnemySpawned);
	}

	//based on choice, may need to choose X placeable items to place an objective
	//tie them to an objective object

	AGameModeBase* MyGameMode = GetWorld()->GetAuthGameMode();

	if (MyGameMode->IsA<ASMainGameMode>())
	{
		//pass that ref to the gamemode object for when the objective is completed
		ASMainGameMode* GM = Cast<ASMainGameMode>(MyGameMode);
		GM->LevelObjective = CurrentObjective;

		//ASStartingSpawnTile* StartingTileEnv = Cast<ASStartingSpawnTile>(SpawnTileEnvRef->AttachedVariant);
		GM->LevelExitRef = SpawnTileEnvRef->LevelExitRef;
		GM->SetLevelSetupComp(this);

		UE_LOG(LogTemp, Log, TEXT("Objective Choosen: %s"), *GM->LevelObjective->ObjectiveName);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Using Wrongly configured gamemode, please check world settings..."));
	}

	UE_LOG(LogTemp, Log, TEXT("--- Objectives choosen and setup objectives ---"));


	//exit level obj setup
	//will be spawning an actor that has a tag marked level exit (use same interaction as the level)
	//TODO: Physical spawn of exit interactable that takes us to train lvl
}

/// <summary>
/// So each tile has all their preplaced markers with local x and y cords
/// Each lvl would have them in a dictionary we can easily look up? or would a 2d array lookup be better?
/// 
/// get noise value of each preplaced item for each tile
/// 
/// </summary>
void ULevelAssetSetupComponent::ActivateItems()
{
	UE_LOG(LogTemp, Log, TEXT(" --- Spawning Pickups! --- "));
	//TODO: Any modifications or different choosing of the weight tables depending on level will go here

	//for each placed variant tile
	for (ASTileVariantEnviornment* TilePlaced : SpawnedVariantsRef)
	{
		//a one time add TilePlaced cleanup to OnCleanupDelegate
		OnCleanupPickups.AddUObject(TilePlaced, &ASTileVariantEnviornment::HandleMarkerCleanup);
		int currentAssetPlacedCount = 0;

		//check each pre-placed pickup
		//mix up pickup placements first
		TArray <UStaticMeshComponent*> PickupsToGoThrough = ReshuffleArray(TilePlaced->PickupPlacements);
		for (UStaticMeshComponent* PossiblePickup : PickupsToGoThrough)
		{
			if (currentAssetPlacedCount > TilePlaced->AssetPlacementCaP)
			{
				break;
			}

			check(PossiblePickup); //trying this check 
			const FVector relativeLocation = PossiblePickup->GetRelativeLocation();
			//UE_LOG(LogTemp, Log, TEXT("Cords: %s"), *relativeLocation.ToString());
			//check noise 
			FVector2D inputConvertionSeedOffset = FVector2D(relativeLocation.X + seedOffset_Items.X, relativeLocation.Y + seedOffset_Items.Y);
			float noiseMeasurement = GetNoiseVec(inputConvertionSeedOffset, MinNoiseItems, MaxNoiseItems);
			//UE_LOG(LogTemp, Log, TEXT("Noise lookup: %f"), noiseMeasurement);

			//threshold check TODO: This will be assigned from 
			float itemThreshold = LocalLevel->GetLocalPickupSpawnLevelThreshold();
			//if meeds threshold, spawn item function for weight lookup and spawn procedure
			
			bool debug = false;
			//TODO: inverse threshold so we go for darker saturation from noise return than the bright color
			if (noiseMeasurement <= itemThreshold)
			{
				//can spawn!
				PlaceItemPickup(PossiblePickup, TilePlaced, PickupsPlaced); //TODO: make blocked out tiles for rest of variants and assign

				//increment counter
				PickupsPlaced++;
				currentAssetPlacedCount++;
				debug = true;

				//UE_LOG(LogTemp, Log, TEXT("Item %d placed"), PickupsPlaced-1);
			}
			if(debug) {
				//UE_LOG(LogTemp, Log, TEXT("item spawned %d: Comparing noise val: %f <= threshold: %f -- Status: %d"), (PickupsPlaced-1), noiseMeasurement, itemThreshold, debug);
			 } else {
				//UE_LOG(LogTemp, Log, TEXT("Comparing noise val: %f <= threshold: %f -- Status: %d"), noiseMeasurement, itemThreshold, debug);
			}
		}
		UE_LOG(LogTemp, Log, TEXT(" --- Tile complete %s, local total: %d --- "), *TilePlaced->GetActorLabel(), currentAssetPlacedCount);
	}
	//each tile has an array of the possible preplaced items
	//if each noise output exceeds a thresholds, we can place
	// placement function
	//count each placement


	//if item meets threshold puts into array to spawn items

	//spawn items in secret room (separate amount, no cap)
	ActivateSecretRoom();

	UE_LOG(LogTemp, Log, TEXT("Done spawning pickup items! Grand Total Placed items in level: %d"), PickupsPlaced);
}

/// <summary>
/// If able to spawn an item, we enter here
/// each item has a weight, that determines who we spawn
/// then spawn item at location
/// 
/// </summary>
void ULevelAssetSetupComponent::PlaceItemPickup(UStaticMeshComponent* PickupMarker, ASTileVariantEnviornment* AttachedTile, int PlacementNum)
{
	FVector spawnLocation = PickupMarker->GetComponentLocation();
	FRotator spawnRotation = PickupMarker->GetComponentRotation();

	//choose random number between 1 and TotalItemWeight
	//int choosenItem = LocalLevel->GameStream.RandRange(0, LevelItemDropWeightTable.Num() - 1);
	float tierChoosenPercent = LocalLevel->GameStream.RandRange(0.0f, 1.0f);

	//percentage ties to tier, get tier and pull randomly from that tier'd item array
	ESpawnTiers ChoosenTier = GetTierOnPercent(tierChoosenPercent);

	//get array of items based on tiers (common has a list of items, uncommon has a list, etc)
	TArray<FItemPickupAsset*> TierItemPool;

	//all assets are in a local var called CurrentLevelItemTierList

	// gunna try this super shorthand via FindByPredicate since its a complicated object (kinda)
	//reference this in the future!
	if (FLocalLevelItemSpawnTiers* FoundMatchingStruct = 
		CurrentLevelItemTierList.FindByPredicate([&](FLocalLevelItemSpawnTiers look) 
		{ 
			return look.CurrentTier == ChoosenTier; })
		)
	{
		TierItemPool = FoundMatchingStruct->ItemPoolAsset;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR!"));
	}

	int ChoosenAssetIndex = LocalLevel->GameStream.RandRange(0, TierItemPool.Num() - 1);
	FItemPickupAsset* AssetToSpawn = TierItemPool[ChoosenAssetIndex];

	TSubclassOf<ASPickupBase> ChoosenAsset = AssetToSpawn->PickupPrefab;

	//TODO: spawn on spawnLocation
	//could take a crazy approach to pickups... What if instead of a floating item in game space,
	//the item is simply in the environment resting on the floor or leaning on a wall, etc. But within
	//a sphere around the intended spawn point. Randomly placed.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<FName> Bundles;
	//calls OnMonsterLoad when loaded, pass along AssetData and FVector to this OnMonsterLoaded function
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ULevelAssetSetupComponent::OnPickupLoaded, AssetToSpawn, spawnLocation, spawnRotation, AttachedTile, PlacementNum);

	UAssetManager* Manager = UAssetManager::GetIfValid();
	Manager->LoadPrimaryAsset(AssetToSpawn->ItemId, Bundles, Delegate);
	
#if WITH_EDITOR
	//debug
	spawnLocation.Z += 700.0f;
	DrawDebugSphere(GetWorld(), spawnLocation, 200.0f, 20, FColor::Emerald, false, 100);
#endif

}

void ULevelAssetSetupComponent::PlaceEnemy(UStaticMeshComponent* PickupMarker, ASTileVariantEnviornment* AttachedTile, int numberSpawned)
{
	FVector spawnLocation = PickupMarker->GetComponentLocation();
	//spawn enemy
	FEnemySpawnInfo* EnemyToSpawnInfo = GetWeightedRandomEnemy();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<FName> Bundles;
	//calls OnMonsterLoad when loaded, pass along AssetData and FVector to this OnMonsterLoaded function
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ULevelAssetSetupComponent::OnEnemyLoaded, EnemyToSpawnInfo, spawnLocation, numberSpawned);

	UAssetManager* Manager = UAssetManager::GetIfValid();
	Manager->LoadPrimaryAsset(EnemyToSpawnInfo->MonsterId, Bundles, Delegate);
	
#if WITH_EDITOR
	//debug
	spawnLocation.Z += 700.0f;
	DrawDebugSphere(GetWorld(), spawnLocation, 200.0f, 20, FColor::Magenta, false, 70);
#endif

	//TODO: link to tile?


}

/// <summary>
/// Input the random float to output the corresponding tier for items (common, uncommon, etc)
/// 
/// Note: The EObjective will only be set directly and not randomly from the objective setup
/// </summary>
/// <param name="inputFloat"></param>
/// <returns></returns>
ESpawnTiers ULevelAssetSetupComponent::GetTierOnPercent(float inputFloat)
{
	if (inputFloat >= 0.0f && inputFloat <= 0.58f) //first 58%
	{
		return ESpawnTiers::ECommon;
	}
	else if (inputFloat < 0.58f && inputFloat <= 0.83f) //next 25%
	{
		return ESpawnTiers::EUncommon;
	} 
	if (inputFloat < 0.83f && inputFloat <= 0.93f) //next 10%
	{
		return ESpawnTiers::ERare;
	}
	if (inputFloat < 0.93f && inputFloat <= 0.98f) //next 5%
	{
		return ESpawnTiers::EEpic;
	}
	if (inputFloat < 0.98f && inputFloat <= 1.0f) //last 2%
	{
		return ESpawnTiers::ELegendary;
	}
	return ESpawnTiers::ECommon;
}

/// <summary>
/// Some preplaced enemy locations allow for cluster spawning for patrols in bigger tiles
/// - Mini bosses can spawn on some objectives (done in objective function)
/// 
/// - Enemies are weighted
/// - 
/// </summary>
void ULevelAssetSetupComponent::ActivateEnemies()
{
	UE_LOG(LogTemp, Log, TEXT(" --- Spawning Enemies! --- "));
	//based on which lvl

	//60 - 40 if we spawn a group vs a single individual enemy (might tie this to lvls as we go on)?
	//TODO: build up squad

	//TODO: Maybe tie this to individual tiles? don't overwhelm an individual tile instead of limited the entire game map
	//will use a point system for determining who to place (taking a different approach than the item spawn)
	//different difficulties will use high budgets (tied to individual tiles but might need to be managed via manager later)

	//for each placed variant tile
	for (ASTileVariantEnviornment* TilePlaced : SpawnedVariantsRef)
	{
		int remainingTileSpawnBudget = TilePlaced->EnemyPlacementBudget;
		int spawnedLocal = 0;
		//a one time add TilePlaced cleanup to OnCleanupDelegate
		OnCleanupPickups.AddUObject(TilePlaced, &ASTileVariantEnviornment::HandleMarkerCleanup);
		
		//check each pre-placed pickup
		//mix up beforehand
		TArray <UStaticMeshComponent*> EnemiesToGoThrough = ReshuffleArray(TilePlaced->EnemyPlacements);
		for (UStaticMeshComponent* PossibleEnemySpawn : EnemiesToGoThrough)
		{
			FEnemySpawnInfo* CurrentEnemy = GetWeightedRandomEnemy();
			if (remainingTileSpawnBudget <= 0)
			{
				break;
			}

			const FVector relativeLocation = PossibleEnemySpawn->GetRelativeLocation();
			//UE_LOG(LogTemp, Log, TEXT("Cords: %s"), *relativeLocation.ToString());
			FVector2D inputConvertionSeedOffset = FVector2D(relativeLocation.X + seedOffset_Items.X, relativeLocation.Y + seedOffset_Items.Y);
			//check noise 
			float noiseMeasurement = GetNoiseVec(inputConvertionSeedOffset, MinNoiseEnemies, MaxNoiseEnemies);

			//threshold check TODO: This will be assigned from 
			float enemyThreshold = LocalLevel->GetLocalEnemySpawnLevelThreshold();
			//if meets threshold, spawn item function for weight lookup and spawn procedure
			if (noiseMeasurement <= enemyThreshold)
			{
				//can spawn!
				PlaceEnemy(PossibleEnemySpawn, TilePlaced, EnemiesPlaced);
				
				remainingTileSpawnBudget -= CurrentEnemy->SpawnCost;
				EnemiesPlaced++;
				spawnedLocal++;
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Tile complete %s, local total: %d"), *TilePlaced->GetActorLabel(), spawnedLocal);
	}

	UE_LOG(LogTemp, Log, TEXT(" --- Done spawning enemies! Grand Total Spawned Enemies in level: %d --- "), EnemiesPlaced);

	//objective setup
	OnEnemySpawnCompletedEvent.Broadcast();
}

/// <summary>
/// Spawn any items that might be in the starting tile
/// Set up the train exit (objective hookup happens in ActiveObjective function)
/// </summary>
void ULevelAssetSetupComponent::SetupStartingTile()
{

	int currentAssetPlacedCount = 0;
	TArray <UStaticMeshComponent*> PickupsToGoThrough = ReshuffleArray(SpawnTileEnvRef->PickupPlacements);
	for (UStaticMeshComponent* PossiblePickup : PickupsToGoThrough)
	{
		if (currentAssetPlacedCount > SpawnTileEnvRef->AssetPlacementCaP)
		{
			break;
		}

		check(PossiblePickup); //trying this check 
		const FVector relativeLocation = PossiblePickup->GetRelativeLocation();
		//UE_LOG(LogTemp, Log, TEXT("Cords: %s"), *relativeLocation.ToString());
		//check noise 
		FVector2D inputConvertionSeedOffset = FVector2D(relativeLocation.X + seedOffset_Items.X, relativeLocation.Y + seedOffset_Items.Y);
		float noiseMeasurement = GetNoiseVec(inputConvertionSeedOffset, MinNoiseItems, MaxNoiseItems);
		//UE_LOG(LogTemp, Log, TEXT("Noise lookup: %f"), noiseMeasurement);

		//threshold check TODO: This will be assigned from 
		float itemThreshold = LocalLevel->GetLocalPickupSpawnLevelThreshold();
		//if meeds threshold, spawn item function for weight lookup and spawn procedure

		bool debug = false;
		//TODO: inverse threshold so we go for darker saturation from noise return than the bright color
		if (noiseMeasurement <= itemThreshold)
		{
			//can spawn!
			PlaceItemPickup(PossiblePickup, SpawnTileEnvRef, PickupsPlaced); //TODO: make blocked out tiles for rest of variants and assign

			//increment counter
			PickupsPlaced++;
			currentAssetPlacedCount++;
			debug = true;

			//UE_LOG(LogTemp, Log, TEXT("Item %d placed"), PickupsPlaced - 1);
		}
		if (debug) {
			//UE_LOG(LogTemp, Log, TEXT("item spawned %d: Comparing noise val: %f <= threshold: %f -- Status: %d"), (PickupsPlaced - 1), noiseMeasurement, itemThreshold, debug);
		}
		else {
			//UE_LOG(LogTemp, Log, TEXT("Comparing noise val: %f <= threshold: %f -- Status: %d"), noiseMeasurement, itemThreshold, debug);
		}
	}
	UE_LOG(LogTemp, Log, TEXT(" --- Tile complete %s, local total: %d --- "), *SpawnTileEnvRef->GetActorLabel(), currentAssetPlacedCount);
}

/// <summary>
/// Enemy pool lookup
/// 
/// TODO: give option for building out mini patrol
/// </summary>
/// <returns></returns>
FEnemySpawnInfo* ULevelAssetSetupComponent::GetWeightedRandomEnemy()
{
	FEnemySpawnInfo* possibleEnemy = nullptr;
	//calculate total weight
	float totalWeight = 0.0f;
	//go through entire pool and add them all up
	for (FEnemySpawnInfo* Enemy : CurrentLevelEnemyList)
	{
		totalWeight += Enemy->Weight;
	}

	//choose random val within totalWeight
	float randomVal = LocalLevel->GameStream.RandRange(0.0f, totalWeight);
	//build a number we can choose TODO: build this out more (or pick different tactic entirely tbh)
	float accumulated = 0.0f;

	//TODO: should i shuffle the enemy list here?

	for (FEnemySpawnInfo* Enemy : CurrentLevelEnemyList)
	{
		accumulated += Enemy->Weight;
		if (randomVal <= accumulated)
		{
			possibleEnemy = Enemy;
			break;
		}
	}
	return possibleEnemy;
}

/// <summary>
/// Inputs a local transform and returns their noise eligibility?
/// 
/// -Set thresholds, spawn items with very high noise values, creating more rare or high priority 
/// hot spots. Low thresholds, spawn more with broader coverage 
///Perlin noise
///calculated offsets
///highest values are choosen from the outputs of plugging in x,y into Perlin noise
///some type of cap for like of output >0.7 then this is a canadate to place
/// 
/// To make sure the noise fits in with our seed:
///The base math noise lookup can't directly apply seeds, but we can simply add our seed to
/// the local x/y input. And to ensure we aren't reusing same values for each tile, we can apply 
/// an addition like (tile number + 100) as well
///calculated offsets
/// 
/// 
///can control density of spread of placements by adjusting noise thresholds and scaling
/// Adjusting scale with Perlin: multiply fvector2d by a float value noise scale. Noise scale > 1.0 
/// increases frequency, creating smaller more detailed patterns. Noise scale < 1.0 decreases 
/// frequency, creating smaller, smoother patterns 
///Low scale = wide smooth, areas of clustering. 
///High scale = fine grated placement, such as detailed enemy patrol points or env props
/// </summary>
/// <returns></returns>
float ULevelAssetSetupComponent::GetNoiseVec(FVector2D inputCords, float MinNoise, float MaxNoise)
{
	//transform vector input by seed to ensure we keep seed influence
	//need to offset by a random number generated by seed (but cant use seed cause can be HUGE)
	//FVector2D inputConvertionSeedOffset(inputCords.X, inputCords.Y); 

	//multiply our cords (with applied offset) to our scale frequency
	//'seedOffset' vect2 is calculated at the beginning and is the same offset used throughout component
   // inputConvertionSeedOffset = (inputCords + seedOffset_Items);
	//UE_LOG(LogTemp, Log, TEXT("Input cords with offset and scale: %s"), *inputConvertionSeedOffset.ToString());

	//normalize
	float cordX = (inputCords.X) / float(LocalLevel->GameMapTextureSize - 1);
	float cordY = (inputCords.Y) / float(LocalLevel->GameMapTextureSize - 1);

	float NoiseLookup = FMath::PerlinNoise2D(FVector2D(cordX, cordY) * LocalLevel->PerlinScaleFreq);

	float NormalizedNoise = (NoiseLookup - MinNoise) / (MaxNoise - MinNoise);

	//UE_LOG(LogTemp, Log, TEXT("not normalized: %f"), NormalizedNoise);
	//float normalizedOutput = FMath::GetMappedRangeValueUnclamped(FVector2D(-1.0f, 1.0f), FVector2D(0.0f, 1.0f), NormalizedNoise);
	//UE_LOG(LogTemp, Log, TEXT("normalized/clamped val: %f"), normalizedOutput);

	return NormalizedNoise;
}

/// <summary>
/// When we finish loading in an enemy, some small setup
/// 
/// Set up ActionComp, monster data for monster, folder path, etc
/// </summary>
/// <param name="EnemySpawnInfo">Input enemy info</param>
/// <param name="SpawnLocation"> Location to spawn enemy that we loaded</param>
void ULevelAssetSetupComponent::OnEnemyLoaded(FEnemySpawnInfo* EnemySpawnInfo, FVector SpawnLocation, int enemyNum)
{
	//LogOnScreen(this, "Finished Loading Monster...", FColor::Green);
	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(EnemySpawnInfo->MonsterId));
		if (MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if (NewBot)
			{
				SpawnedEnemiesInLevel.Add(NewBot);
				FString EnemyName = EnemySpawnInfo->EnemyName + "_" + FString::FromInt(enemyNum);
				NewBot->SetActorLabel(EnemyName);

#if WITH_EDITOR
				NewBot->SetFolderPath(TileManagerRef->EnemySubFolderName);
#endif

				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<USAction>ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
	}
}

/// <summary>
/// Runs when pickup has loaded in. Pass in arguements when we start the load.
/// </summary>
/// <param name="ItemSpawnInfo"></param>
/// <param name="SpawnLocation"></param>
/// <param name="spawnRotation"></param>
/// <param name="AttachedTile"></param>
void ULevelAssetSetupComponent::OnPickupLoaded(FItemPickupAsset* ItemSpawnInfo, FVector SpawnLocation, FRotator spawnRotation ,ASTileVariantEnviornment* AttachedTile, int itemNum)
{
	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		//ASPickupBase* ItemData = Cast<ASPickupBase>(Manager->GetPrimaryAssetObject(ItemSpawnInfo->ItemId));
		//if (ItemData)
		//{
			ASPickupBase* NewPickup = GetWorld()->SpawnActor<ASPickupBase>(ItemSpawnInfo->PickupPrefab, SpawnLocation, spawnRotation);
			if (NewPickup)
			{
				SpawnedPickupsInLevel.Add(NewPickup);
				FString ItemName = ItemSpawnInfo->ItemName + "_" + FString::FromInt(itemNum) + "_" + UEnum::GetValueAsString(AttachedTile->TileVariDefinition->EVariantSize);
				NewPickup->SetActorLabel(ItemName);

#if WITH_EDITOR
				NewPickup->SetFolderPath(TileManagerRef->AssetSubFolderName);
#endif

				//LogOnScreen(this, FString::Printf(TEXT("Spawned Enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

			}
		//}
	}
}

/// <summary>
/// Turn on and off the debug floor
/// Create PerlinNoise texture of floor
/// Apply perlin noise texture to floor
/// </summary>
void ULevelAssetSetupComponent::SetUpDebugPerlinNoise()
{
	UStaticMeshComponent* DebugFloorSMComp = DebugPerlinNoiseFloor->FindComponentByClass<UStaticMeshComponent>();
	DebugFloorSMComp->ToggleVisibility(ActivateDebugFloorPerlinNoise); //set it to be visible
	//DebugFloorSMComp->GetStaticMesh()->GetBounds();
	FBoxSphereBounds Bounds = DebugFloorSMComp->Bounds;

	//(Title length * Number of tiles) + (Wall buffer length * number of buffers in direction)
	//(35 * 5) + (1 * 5) = 180 (for scale size)
	//normal tile is 35 x 35 (with wall buffers of 1) so having 5 tiles gives us a floor size of 179 x 179
	UTexture2D* FloorPerlinTexture = DebugCreatePerlinNoiseTexture(LocalLevel->GameMapTextureSize, Bounds); //but cant exceed cap of 16,384 x 16,384

	//create our runtime material to apply texture to
	UMaterialInstanceDynamic* FloorMID = DebugFloorSMComp->CreateAndSetMaterialInstanceDynamic(0); //0 is material slot on mesh

	FloorMID->SetTextureParameterValue("PerlinNoiseTexture", FloorPerlinTexture);
}

/// <summary>
/// WARNING: THIS IS VERY INTENSE TO RUN SO KEEP THIS OFF WHEN I DONT NEED IT
/// Going to attempt to generate 2d texture for floor to showcase our perlin noise (for easy debugging and tweaking)
/// Current idea of how to do this: create 2d texture with resolution (still tbd on that, how many units is our game map rn? x,y),
/// and then for each cord, convert to noise cord lookup and set that color scale value based on returned value
/// 
/// NOTE: Instead of setting this texture to each floor or each tile, make one big floor that is the proper size and apply it to that, 
/// will be easier. This var is in this component called: 'DebugPerlinNoiseFloor'
/// </summary>
/// <param name="size"> width and length of texture (aka our floor actor we are using). aka resolution </param>
/// <param name="NoiseScale"> multiplier or frequency of noise aka our assetPlacementScaleFreq we have used </param>
/// <param name="OffsetX"> offset cord for x cord on texture we are setting </param>
/// <param name="OffsetY"> offset cord for Y cord on texture we are setting </param>
UTexture2D* ULevelAssetSetupComponent::DebugCreatePerlinNoiseTexture(int32 size, FBoxSphereBounds Bounds)
{
	//to better map
	//world bounds in units is 18000 x 18000 for BASE MAP (not secret room or entrance)
	//origin - extension = min corner
	//origin + extension = max corner
	float WorldMinX = Bounds.Origin.X - Bounds.BoxExtent.X;
	float WorldMinY = Bounds.Origin.X + Bounds.BoxExtent.X;
	float WorldMaxX = Bounds.Origin.Y - Bounds.BoxExtent.Y;
	float WorldMaxY = Bounds.Origin.Y + Bounds.BoxExtent.Y;
	//UE_LOG(LogTemp, Log, TEXT("Min X: %f"), WorldMinX);
	//UE_LOG(LogTemp, Log, TEXT("Min Y: %f"), WorldMinY);
	//UE_LOG(LogTemp, Log, TEXT("Max X: %f"), WorldMaxX);
	//UE_LOG(LogTemp, Log, TEXT("Max Y: %f"), WorldMaxY);

	//create blank texture
	UTexture2D* Texture = UTexture2D::CreateTransient(size, size, PF_B8G8R8A8); //PF_B8G8R8A8 is a pixel format? using BGR instead of RGB
	//blue has 8 bits, g has 8 bits, r has 8 buts and alpha has 8 bits. looks like we are setting the properties of this texture
	
	//tell unreal not to generate mipmaps, this should be pixel-perfect visualization of our debug perlin noise
	Texture->MipGenSettings = TMGS_NoMipmaps; //also is intense to keep on so why bother
	Texture->SRGB = false; //treat this as linear data not color data (because we are using perlin noise). keep more numeric

	TArray<FColor> Pixels; //set array of pixels to be our texture
	Pixels.SetNum(size * size);

	//going with an auto normalize technique to pull us out of the gray scale
	//might be intensive to run, this may need to be moved to run and used before we place items btw
	//reminder: scale should make our min and max as close as possible to -1,1

	const FVector2D Offset = (DebugTextureToggle) ? seedOffset_Items : seedOffset_Enemies;
	const float MinNoise = (DebugTextureToggle) ? MinNoiseItems : MinNoiseEnemies;
	const float MaxNoise = (DebugTextureToggle) ? MaxNoiseItems : MaxNoiseEnemies;

	//go through all the spots and color them (all the pixels... pain)
	for (int y = 0; y < size; y++)
	{
		//for each y row, go through x column
		for (int x = 0; x < size; x++)
		{
			//convert pixel cords to world cords an attempt (please optimize this later omg)
			//normalized UV?
			float a = (x + Offset.X) / float(size - 1);
			float b = (y + Offset.Y) / float(size - 1);

			float NoiseLookup = FMath::PerlinNoise2D(FVector2D(a, b) * LocalLevel->PerlinScaleFreq); //get Perlin noise val
			
			//global normalization
			float NormalizedNoise = (NoiseLookup - MinNoise) / (MaxNoise - MinNoise);
			float clampedNoise = FMath::Clamp(NormalizedNoise, 0.0f, 1.0f);

			//clamp value to be used as color
			//use updated clamp w/ normalized noise values to get me the hell out of mid-gray band PLEASE
			int valueColor = FMath::Clamp( NormalizedNoise * 255.0f, 0, 255); //create color value via clamp (255 for color value)

			//debug
			//if (x % 200 == 0 && y % 200 == 0)
			//{
				//UE_LOG(LogTemp, Log, TEXT("unnormalized values: x: %f, y: %f"), a, b);
				//UE_LOG(LogTemp, Log, TEXT("scaled: %f, %f"), scaledX, scaledY);
				//UE_LOG(LogTemp, Log, TEXT("noise normalized: %f ==> color clamp (0-255): %d"), NormalizedNoise, valueColor);
			//}

			Pixels[y * size + x] = FColor(valueColor, valueColor, valueColor, 255); // set as all values so its black and white
		}
	}

	//MipMaps are lower res versions of texture
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];//mip 0 is full resolution (1 is half, etc)

	//to protect data memory (concurrency reasons)
	//raw pointer in pixel mem, bulkdata is actual bytes and lock is asking for permission to edit this data
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE); 

	//copy the pixels into texture data 
	FMemory::Memcpy(Data, Pixels.GetData(), Pixels.Num() * sizeof(FColor));

	//apply and lock back up?
	Mip.BulkData.Unlock();
	Texture->CompressionSettings = TC_Grayscale; 
	Texture->NeverStream = true; //please unreal don't compress or muck up this texture
	Texture->Filter = TF_Nearest;
	Texture->UpdateResource(); //apply updates to texture we have made

	return Texture;
}

/// <summary>
/// Populate grid with assets, called from TileGeneration once it is done setting up
/// 
/// - Handles setup of for this component, setup for big tiles, level key, activating secret room, 
/// objective, items, enemies
/// 
/// 
/// All items have a corresponding weight for given lvl
/// - Weight random distribution for which item we spawn
/// - Perlin noise for figuring out which premarked locations on the map we end up using
/// - each placed tile will have 'markers' for where it would best make sense to place items
/// - using a Perlin noise we can look at see which we can use, IE hotspots
/// - Tiles have caps of how many items we can place
/// 
/// - Each tile has preplaced actors for enemies and items. These guys have local transforms. 
/// The Perlin noise generator looks at these in order to map the locations to places found in 
/// the Perlin noise
/// 
/// </summary>
void ULevelAssetSetupComponent::PopulateGridAssets()
{
	SetupLevelAssetComponent();
	
	//do items first then focus on choosing and creating objectives
	ActivateObjectives();

	ActivateItems();

	ActivateEnemies();

	SetupStartingTile();

	CleanupAllItemPickups();

	//Finished grid branch creation
	UE_LOG(LogTemp, Log, TEXT("\n================= Finished ========================="));
}

