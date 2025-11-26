// Fill out your copyright notice in the Description page of Project Settings.


#include "ULevelAssetSetupComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UTileGridBranchComponent.h"

//Note: This component is purely for placement and choosing of level assets (IE Items and objectives?)
// and enemies

//all the data for level will be kept in local level



//TODO: Visualize the noise map. Material debugging, create mat that maps Perlin noise 
// to a texture or world surface (like the floor?).Use debug spheres or other markers to 
// visualize valid locations based on thresholds 


/// - TODO: These are like pickups right? Lets inherit from our pickup class we already have looked at
/// Will for now make the spawned items populate an array of pickups. Will make blockouts for lvl2 to include
/// the preplaced 'markers'.
/// 1. Make variant blockouts for example lvl. Include the preplacements of items and enemies
/// 2. Perlin noise system and how to determine which places to use.
/// 3. weight table for which items to spawn
/// 4. Spawn item function

// Sets default values for this component's properties
ULevelAssetSetupComponent::ULevelAssetSetupComponent()
{

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
	//set up the asset count array

	//set up the big asset count array

	//select which tier our level is


}

void ULevelAssetSetupComponent::GridAnalysis()
{

}

void ULevelAssetSetupComponent::ActivateLevelKey()
{

}

void ULevelAssetSetupComponent::ActivateSecretRoom()
{

}

void ULevelAssetSetupComponent::ActivateObjectives()
{

}

/// <summary>
/// So each tile has all their preplaced markers with local x and y cords
/// Each lvl would have them in a dictionary we can easily look up? or would a 2d array lookup be better?
/// 
/// 
/// 
/// </summary>
void ULevelAssetSetupComponent::ActivateItems()
{
	//get noise value of each preplaced item for each tile
	UTileGridBranchComponent* GridBranchCompRef = TileManagerRef->GetGridBranchComp();

	//go through all placed tiles (secret room should be separate)
	TArray <ASTileVariantEnviornment*> SpawnedVariantsRef = GridBranchCompRef->GetSpawnedVariantTiles();

	//TODO: Any modifications or different choosing of the weight tables depending on level will go here

	//prep weight table
	for (FItemPickupAsset Pickup : ItemData->ItemPickupTable)
	{
		for (int TimesToAdd = 0; TimesToAdd < Pickup.ItemWeight; TimesToAdd++)
		{
			LevelItemDropWeightTable.Add(Pickup);
		}
	}

	//for each placed variant tile
	for (ASTileVariantEnviornment* TilePlaced : SpawnedVariantsRef)
	{
		//a one time add TilePlaced cleanup to OnCleanupDelegate
		OnCleanupPickups.AddUObject(TilePlaced, &ASTileVariantEnviornment::HandleMarkerCleanup);

		//check each pre-placed pickup
		for (UStaticMeshComponent* PossiblePickup : TilePlaced->PickupPlacements)
		{
			check(PossiblePickup); //trying this check 
			const FVector relativeLocation = PossiblePickup->GetRelativeLocation();
			UE_LOG(LogTemp, Log, TEXT("Cords: %s"), *relativeLocation.ToString());
			//check noise 
			float noiseMeasurement = GetNoiseVec(relativeLocation);
			UE_LOG(LogTemp, Log, TEXT("Noise lookup: %f"), noiseMeasurement);

			//threshold check TODO: This will be assigned from 
			float itemThreshold = LocalLevel->GetLocalPickupSpawnLevelThreshold();
			//if meeds threshold, spawn item function for weight lookup and spawn procedure
			UE_LOG(LogTemp, Log, TEXT("Comparing: %f to threshold: %f"), noiseMeasurement, itemThreshold);
			if (noiseMeasurement <= itemThreshold)
			{
				//can spawn!
				PlaceItemPickup(PossiblePickup); //TODO: make blocked out tiles for rest of variants and assign

				//increment counter
				PickupsPlaced++;
			}

		}

	}
	//each tile has an array of the possible preplaced items
	//if each noise output exceeds a thresholds, we can place
	// placement function
	//count each placement


	//if item meets threshold puts into array to spawn items

	//spawn items?
}

/// <summary>
/// If able to spawn an item, we enter here
/// each item has a weight, that determines who we spawn
/// then spawn item at location
/// 
/// </summary>
void ULevelAssetSetupComponent::PlaceItemPickup(UStaticMeshComponent* PickupMarker)
{
	FVector spawnLocation = PickupMarker->GetComponentLocation();

	//choose random number between 1 and TotalItemWeight
	int choosenItem = LocalLevel->GameStream.RandRange(0, LevelItemDropWeightTable.Num() - 1);

	FItemPickupAsset ChoosenAsset = LevelItemDropWeightTable[choosenItem];

	
	//TODO: spawn on spawnLocation
	//could take a crazy approach to pickups... What if instead of a floating item in game space,
	//the item is simply in the environment resting on the floor or leaning on a wall, etc. But within
	//a sphere around the intended spawn point. Randomly placed.



	spawnLocation.Z += 700.0f;
	DrawDebugSphere(GetWorld(), spawnLocation, 200.0f, 20, FColor::Emerald, false, 100);
}

/// <summary>
/// Some preplaced enemy locations allow for cluster spawning for patrols in bigger tiles
/// - Mini bosses can spawn on some objectives
/// </summary>
void ULevelAssetSetupComponent::ActivateEnemies()
{

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
float ULevelAssetSetupComponent::GetNoiseVec(FVector inputCords)
{

	//transform vector input by seed to ensure we keep seed influence
	//need to offset by a random number generated by seed (but cant use seed cause can be HUGE)
	FVector2D inputConvertionSeedOffset(inputCords.X, inputCords.Y); 
	//UE_LOG(LogTemp, Log, TEXT("Cords: %s"), *inputConvertionSeedOffset.ToString());

	float scaleFreq = 0.01;

	FVector2D seedOffset(LocalLevel->GameStream.RandRange(-1000, 1000), LocalLevel->GameStream.RandRange(-1000, 1000));

    inputConvertionSeedOffset = inputConvertionSeedOffset + seedOffset * scaleFreq;
	//UE_LOG(LogTemp, Log, TEXT("Input cords with offset and scale: %s"), *inputConvertionSeedOffset.ToString());
	
	float noiseOutput = FMath::PerlinNoise2D(inputConvertionSeedOffset);
	//UE_LOG(LogTemp, Log, TEXT("not normalized: %f"), noiseOutput);
	//normalize (so between 0 and 1) not within -1,1
	//float normalizedOutput = UKismetMathLibrary::NormalizeToRange(noiseOutput, 0.0f, 1.0f); //is this not what i thought it was?
	
	//float normalize2 = (noiseOutput - 0.0f) / (1.0 - 0.0f);
	//UE_LOG(LogTemp, Log, TEXT("normallized1: %f"), normalize2);

	//float normalize3 = FMath::GetMappedRangeValueClamped(FVector2D(-1.0f, 1.0f), FVector2D(0.0f, 1.0f), noiseOutput);
	//UE_LOG(LogTemp, Log, TEXT("normallized2: %f"), normalize3);

	float normalizedOutput = FMath::GetMappedRangeValueUnclamped(FVector2D(-1.0f, 1.0f), FVector2D(0.0f, 1.0f), noiseOutput);
	//UE_LOG(LogTemp, Log, TEXT("normallized3: %f"), normalize4);

	//float normalize5 = (noiseOutput + 1.0f) / 2.0f;
	//UE_LOG(LogTemp, Log, TEXT("normallized4: %f"), normalize5);

	return normalizedOutput;
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

	
	//UE_LOG(LogTemp, Log, TEXT("noise test (1, 1): %d"), GetNoiseVec((1, 1)));
	//UE_LOG(LogTemp, Log, TEXT("noise test2 (1, 4): %d"), GetNoiseVec((1, 4)));
	//UE_LOG(LogTemp, Log, TEXT("noise test3 (3, 7): %d"), GetNoiseVec((3, 7)));

	//do items first then focus on choosiung and creating objectives
	ActivateObjectives();

	ActivateItems();

	ActivateEnemies();

	CleanupAllItemPickups();

	//Finished grid branch creation
	UE_LOG(LogTemp, Log, TEXT("\n\n================= Finished ========================="));
}

