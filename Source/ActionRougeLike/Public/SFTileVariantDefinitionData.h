// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "STileVariantEnviornment.h"
#include "Enviornment/TileGeneration/STileDoor.h"
#include "ASTileWall.h"
#include "SFTileVariantDefinitionData.generated.h"

UENUM(BlueprintType)
enum class ETileSizeVariant : uint8 {
	ET1x1 UMETA(DisplayName = "TileVariant_1X1"),
	ET2x1 UMETA(DisplayName = "TileVariant_2x1"),
	ET2x2 UMETA(DisplayName = "TileVariant_2x2"),
	ET3x1 UMETA(DisplayName = "TileVariant_3x1"),
	ET3x2 UMETA(DisplayName = "TileVariant_3x2"),
	ET4x2 UMETA(DisplayName = "TileVariant_4x2"),
	ET4x3 UMETA(DisplayName = "TileVariant_4x3"),
	ET4x4 UMETA(DisplayName = "TileVariant_4x4")
};

/// <summary>
/// essentially a FIntPoint of FIntPoints
/// </summary>
USTRUCT(BlueprintType)
struct FIntPointPair {
	GENERATED_BODY()

	public:
	UPROPERTY(EditAnywhere)
	FIntPoint StartCords;

	UPROPERTY(EditAnywhere)
	FIntPoint EndCords;

	//constructor
	FIntPointPair()
	{

	}

	FIntPointPair(FIntPoint startCord, FIntPoint endCord) //can also looked at as (Y cord : X Cord)
	{
		StartCords = startCord;
		EndCords = endCord;
	}

};

USTRUCT(BlueprintType)
struct FTileVariantDefinitionRow
{
	GENERATED_BODY()

public:
	//variant type
	UPROPERTY(EditAnywhere)
	TArray<USFTileVariantDefinitionData*> Columns;

	UPROPERTY(EditAnywhere)
	int Min;

	UPROPERTY(EditAnywhere)
	int Max;

	UPROPERTY(EditAnywhere)
	FString Note;

	//Constructor 
	FTileVariantDefinitionRow(TArray<USFTileVariantDefinitionData*> columns, int min, int max)
	{
		Columns = columns;
		Min = min;
		Max = max;
	}

	FTileVariantDefinitionRow()
	{
		Max = 1;
	}

};

USTRUCT(BlueprintType)
struct FTileVariantSetup_PlugTileSaveInfo
{
	GENERATED_BODY()

	public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ASTileDoor*> DoorsArray;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ASTileWall*> WallArray;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<ASTile*> TileArray;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int TransformDirectionSetup;

};


/// <summary>
/// Each 'flavor' or rotated offset is saved here to be used in the plug and chug algorithm
/// </summary>
USTRUCT(BlueprintType)
struct FVariantOffsetTransforms_Rotates
{
	GENERATED_BODY()

public:

    //1 = left neighbor, 2 = right neighbor, 3 = up neighbor, 4 = down neighbor
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FIntPoint>DirectionsRotations;

	UPROPERTY(EditAnywhere)
	TArray<FIntPointPair> CordsSideToCheck;

	//maybe this be an int?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TransformDirectionRotation;

	FVariantOffsetTransforms_Rotates()
	{
		//Transforms_flavor = {1,2,3,4};

	}

	FVariantOffsetTransforms_Rotates(TArray<FIntPoint> transforms, int direction)
	{
		DirectionsRotations = transforms;
		TransformDirectionRotation = direction;
	}

};

/// <summary>
/// We have a starting point sides to check (aka the connecting sides of each tile per variant)
/// If the variant is rotated (either 3 other rotations), therefore we have to have rotated cords
/// 
/// Should be similar to how we set VariantPaths
/// </summary>
USTRUCT(BlueprintType)
struct FVariantOffsetTransforms_SidesToCheckOffsetsRotations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FIntPointPair> ConnectingSideOffset;


	FVariantOffsetTransforms_SidesToCheckOffsetsRotations(TArray<FIntPointPair> SidesToCheckOffsets, int direction)
	{
		ConnectingSideOffset = SidesToCheckOffsets;
	}

	FVariantOffsetTransforms_SidesToCheckOffsetsRotations()
	{

	}


};

/*
* (0,0) is considered anchor tile
* Idea: define one variant direction, then apply a transformation on it and rotate it the other 3 directions
* - Define basic shape definition
* - rotate offsets dynamically (so 90, 180 and 270), save those out maybe? maybe precomputed and saved in editor?
* - use these values in placement checks
* new data asset or maybe the basic shape definitions in a Datatable or UDataasset like before
* 
* Idea 2: define all of them here (way to long for end ones)
* ET1x1 UMETA(DisplayName = "TileVariant_1X1"),
* - none
* ET2x1 UMETA(DisplayName = "TileVariant_2x1"), (only one check, 2 tiles total, 4 permutations)
* - up
* - down
* - left
* - right
* ET2x2 UMETA(DisplayName = "TileVariant_2x2"), (only 3 checks, 4 tiles total, )
* - up, left, down, right
* - down, left
* - left,
* - right,
* ET3x1 UMETA(DisplayName = "TileVariant_3x1"), (only 2 checks, 3 tiles total)
* - up, 
* - down,
* - left,
* - right,
* ET3x2 UMETA(DisplayName = "TileVariant_3x2"), (only 2 checks, 6 tiles total)
* - up, 
* - down,
* - left,
* - right,
* ET4x2 UMETA(DisplayName = "TileVariant_4x2"), (only 2 checks, 8 tiles total)
* - up, 
* - down,
* - left,
* - right,
* ET4x3 UMETA(DisplayName = "TileVariant_4x3"), (only 2 checks, 12 tiles total)
* - up, 
* - down,
* - left,
* - right,
* ET4x4 UMETA(DisplayName = "TileVariant_4x4"), (only 2 checks, 16 tiles total)
* - up, 
* - down,
* - left,
* - right,
*/

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USFTileVariantDefinitionData : public UDataAsset
{
	GENERATED_BODY()
	
	public:

	USFTileVariantDefinitionData();

	// ---------------------------------
	// ------- Public Functions --------
	// ---------------------------------

	UFUNCTION(BlueprintCallable)
	TArray<FIntPoint> RotateOffsets(TArray<FIntPoint> OriginalOffsets, int32 RotationStepsClockwise);

	UFUNCTION(BlueprintCallable)
	TArray<FIntPointPair> RotateConnectedSides(TArray<FIntPointPair> OriginalOffsets, int32 RotationStepsClockwise);

	UFUNCTION(BlueprintCallable)
	void SetVariantPaths();

	// ---------------------------------
	// -------- Public Variables -------
	// ---------------------------------

	//variant type
	UPROPERTY(EditAnywhere)
	ETileSizeVariant EVariantSize;

	//could define as a FIntPoint? so like a tuple?
	UPROPERTY(EditAnywhere)
	FIntPoint Size;

	//priority (bigger tiles should only be used once if at all and should be the hardest to find spots for
	//closer the number to 10, the higher the priority, -1 means excluded
	UPROPERTY(EditAnywhere)
	int Priority;

	UPROPERTY(EditAnywhere)
	int MinorMin;

	UPROPERTY(EditAnywhere)
	int MinorMax;

	UPROPERTY(EditAnywhere)
	bool bIsSingleVariant;

	//offset transforms, original (unrotated)
	UPROPERTY(EditAnywhere)
	TArray<FIntPoint> TileVariantOffsets; // = {
	//	FIntPoint(0, 0),  // Origin (assumed clear)
	//	FIntPoint(1, 0),  // Right
	//	FIntPoint(0, 1),  // Down
	//	FIntPoint(1, 1)   // Bottom-right
	//};

	//unrotated as well
	UPROPERTY(EditAnywhere)
	TArray<FIntPointPair> SidesToCheckOffsets;


	UPROPERTY(EditAnywhere)
	TArray<FVariantOffsetTransforms_SidesToCheckOffsetsRotations> SidesToCheckRotation;

	UPROPERTY(EditAnywhere)
	int RotationCheckCounter;

	//possible variants that get choosen randomly when spawned (not spawned yet and pulled from locallevel)
	//the prefabs themselves to spawn
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ASTileVariantEnviornment>>  TileVariantEnviornments;

	UPROPERTY(EditAnywhere)
	TArray<FVariantOffsetTransforms_Rotates> VariantPaths;

	//Constructor 
	USFTileVariantDefinitionData(ETileSizeVariant variant, FIntPoint size, int priority, int min, int max)
	{
		EVariantSize = variant;
		Size = size;
		Priority = priority;
		MinorMin = min;
		MinorMax = max;
	}

};
