// Fill out your copyright notice in the Description page of Project Settings.


#include "SFTileVariantDefinitionData.h"


USFTileVariantDefinitionData::USFTileVariantDefinitionData()
{
	
	minorMax = 1;
	//SetVariantPaths();
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
				StartCord = LocalOffet.startCords;
				EndCord = LocalOffet.endCords;
			break;
			case 1:
			//90 deg
				StartCord = FIntPoint(LocalOffet.startCords.Y, -LocalOffet.startCords.X);
				EndCord = FIntPoint(LocalOffet.endCords.Y, -LocalOffet.endCords.X);
			break;
			case 2: //this should be neg for both right?
			//180 deg
				StartCord = FIntPoint(-LocalOffet.startCords.X, -LocalOffet.startCords.Y);
				EndCord = FIntPoint(LocalOffet.endCords.Y, LocalOffet.endCords.X);
			break;
			case 3: //flipped from case 1 right?
			//270 deg
				StartCord = FIntPoint(-LocalOffet.startCords.Y, LocalOffet.startCords.X);
				EndCord = FIntPoint(-LocalOffet.endCords.Y, LocalOffet.endCords.X);
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

void USFTileVariantDefinitionData::SetVariantPaths()
{
	if(VariantPaths.IsEmpty()) {
		//UE_LOG(LogTemp, Error, TEXT("Array isn't empty? why this run more than once on this guy?"));
		
		VariantPaths.Add(FVariantOffsetTransforms_Rotates(Offsets, 0)); //offsets is empty at constructors

		if (Variant != ETileSizeVariant::ET1x1) {
			for (int dir = 1; dir < 4; dir++)
			{
				TArray<FIntPoint> Rotated = RotateOffsets(Offsets, dir);

			//get rotated sides to check also


				FVariantOffsetTransforms_Rotates offset = FVariantOffsetTransforms_Rotates(Rotated, dir);
				VariantPaths.Add(offset);
			}
		}
	}

	if(SidesToCheckRotation.IsEmpty()) {
		//rotate sidestocheck also
		SidesToCheckRotation.Add(FVariantOffsetTransforms_SidesToCheckOffsetsRotations(SidesToCheckOffsets, 0));

		if (Variant != ETileSizeVariant::ET1x1) {
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

