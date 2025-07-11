// Fill out your copyright notice in the Description page of Project Settings.


#include "SFTileVariantDefinitionData.h"


USFTileVariantDefinitionData::USFTileVariantDefinitionData()
{
	
	minorMax = 1;
	

	VariantPaths.Add(FVariantOffsetTransforms_Rotates(Offsets, 0));

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
		FIntPoint Transformed = LocalOffet;

		//each number is one 90 degree rotation then add the result to the array
		for (int index = 0; index < RotationStepsClockwise; index++)
		{
			Transformed = FIntPoint(-Transformed.Y, Transformed.X);
		}
		Rotated.Add(Transformed);
	}
	return Rotated;
}
