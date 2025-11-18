// Fill out your copyright notice in the Description page of Project Settings.


#include "SFTileVariantDefinitionData.h"


USFTileVariantDefinitionData::USFTileVariantDefinitionData()
{
	MinorMax = 1;
}

/// <summary>
/// for rotating each side for the transformations
/// 
/// each side is fed in 1 - 3
/// 1 = 90 degrees
/// 2 = 180 degrees
/// 3 = 270 degrees
/// each offset array is saved out on construction and used when we check if these variants fit
/// </summary>
/// <param name="OriginalOffsets"></param>
/// <param name="RotationStepsClockwise"></param>
/// <returns></returns>
TArray<FIntPoint> USFTileVariantDefinitionData::RotateOffsets(TArray<FIntPoint> OriginalOffsets, int RotationStepsClockwise)
{
	TArray<FIntPoint> Rotated;
	//for each transform
	for (FIntPoint& LocalOffet : OriginalOffsets)
	{
		FIntPoint Transformed;
		switch (RotationStepsClockwise)
		{
			case 0:
			//relative
				Transformed = LocalOffet;
			break;
			case 1:
			//90 deg
				Transformed = FIntPoint(LocalOffet.Y, -LocalOffet.X);
			break;
			case 2: //this should be neg for both right?
			//180 deg
				Transformed = FIntPoint(-LocalOffet.X, -LocalOffet.Y);
			break;
			case 3: //flipped from case 1 right?
			//270 deg
				Transformed = FIntPoint(-LocalOffet.Y, LocalOffet.X);
			break;
			default:	
				UE_LOG(LogTemp, Error, TEXT("oppsie"));
			break;
		}

		Rotated.Add(Transformed);
	}
	return Rotated;
}

/// <summary>
/// for rotating each connection between variant tiles for the transformations
/// 
/// each side is fed in 1 - 3
/// 1 = 90 degrees (y, -x)
/// 2 = 180 degrees (-x,-y)
/// 3 = 270 degrees (-y,x)
/// each offset array is saved out on construction and used when we check if these variants fit
/// </summary>
/// <param name="OriginalOffsets"></param>
/// <param name="RotationStepsClockwise"></param>
/// <returns></returns>
TArray<FIntPointPair> USFTileVariantDefinitionData::RotateConnectedSides(TArray<FIntPointPair> OriginalOffsets, int32 RotationStepsClockwise)
{
	TArray<FIntPointPair> Rotated;

	//for each transform
	for (FIntPointPair& LocalOffet : OriginalOffsets)
	{
		FIntPointPair Transformed;
		 //test it rotates both okayFIntPoint StartCord 
		FIntPoint StartCord;
		FIntPoint EndCord;

		//new approach, convert to radians then do maths to make it back to cords to apply to array
		switch (RotationStepsClockwise)
		{
			case 0:
			//relative
				StartCord = LocalOffet.StartCords;
				EndCord = LocalOffet.EndCords;
			break;
			case 1:
			//90 deg
				StartCord = FIntPoint(LocalOffet.StartCords.Y, -LocalOffet.StartCords.X);
				EndCord = FIntPoint(LocalOffet.EndCords.Y, -LocalOffet.EndCords.X);
			break;
			case 2: //this should be neg for both right?
			//180 deg
				StartCord = FIntPoint(-LocalOffet.StartCords.X, -LocalOffet.StartCords.Y);
				EndCord = FIntPoint(-LocalOffet.EndCords.X, -LocalOffet.EndCords.Y);
			break;
			case 3: //flipped from case 1 right?
			//270 deg
				StartCord = FIntPoint(-LocalOffet.StartCords.Y, LocalOffet.StartCords.X);
				EndCord = FIntPoint(-LocalOffet.EndCords.Y, LocalOffet.EndCords.X);
			break;
			default:
				UE_LOG(LogTemp, Error, TEXT("oppsie"));
				break;
		}
		Transformed = FIntPointPair(StartCord, EndCord); //TODO: incorporate FIntPointPair with this algorithm
		Rotated.Add(Transformed);
	}
	return Rotated;

}

//This populates the VariantPaths and SidesToChechRotate on the data objects if not set up right, should only have to
//be run once. If the counts for each are not 4 then we gotta remake them. Each direction is represented.
void USFTileVariantDefinitionData::SetVariantPaths()
{
	//if not empty and size is 4 or greater, start over
	if (VariantPaths.Num() != 4)
	{
		VariantPaths.Empty();
	}

	
	if(VariantPaths.IsEmpty()) {
		//UE_LOG(LogTemp, Error, TEXT("Array isn't empty? why this run more than once on this guy?"));
		VariantPaths.Add(FVariantOffsetTransforms_Rotates(TileVariantOffsets, 0)); //offsets is empty at constructors

		if (EVariantSize != ETileSizeVariant::ET1x1) {
			for (int dir = 1; dir < 4; dir++)
			{
				TArray<FIntPoint> Rotated = RotateOffsets(TileVariantOffsets, dir);

				//get rotated sides to check also

				FVariantOffsetTransforms_Rotates offset = FVariantOffsetTransforms_Rotates(Rotated, dir);
				VariantPaths.Add(offset);
			}
		}
	}
	
	if (SidesToCheckRotation.Num() != 4)
	{
		SidesToCheckRotation.Empty();
	}

	if(SidesToCheckRotation.IsEmpty()) {
		//rotate sidestocheck also
		SidesToCheckRotation.Add(FVariantOffsetTransforms_SidesToCheckOffsetsRotations(SidesToCheckOffsets, 0));

		if (EVariantSize != ETileSizeVariant::ET1x1) {
			for (int dir = 1; dir < 4; dir++)
			{
				TArray<FIntPointPair> RotatedSidesToCheck = RotateConnectedSides(SidesToCheckOffsets, dir);

				//get rotated sides to check also

				FVariantOffsetTransforms_SidesToCheckOffsetsRotations offset = FVariantOffsetTransforms_SidesToCheckOffsetsRotations(RotatedSidesToCheck, dir);
				SidesToCheckRotation.Add(offset);
			}
		}
	}
}

