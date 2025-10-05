// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelVariantPresetsData.generated.h"

class ASTileVariantEnviornment;

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ULevelVariantPresetsData : public UDataAsset
{
	GENERATED_BODY()
	
	public:
	//level 1

	UPROPERTY(EditAnywhere, Category = "Lvl1")
	TArray<TSubclassOf<ASTileVariantEnviornment>> OnexOneEnvVariants_lvl1;

	UPROPERTY(EditAnywhere, Category = "Lvl1")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxOneEnvVariants_lvl1;

	UPROPERTY(EditAnywhere, Category = "Lvl1")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxTwoEnvVariants_lvl1;

	UPROPERTY(EditAnywhere, Category = "Lvl1")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexOneEnvVariants_lvl1;

	UPROPERTY(EditAnywhere, Category = "Lvl1")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexTwoEnvVariants_lvl1;

	UPROPERTY(EditAnywhere, Category = "Lvl1")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxTwoEnvVariants_lvl1;

	UPROPERTY(EditAnywhere, Category = "Lvl1")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxThreeEnvVariants_lvl1;

	UPROPERTY(EditAnywhere, Category = "Lvl1")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxFourEnvVariants_lvl1;

	//level 2

	UPROPERTY(EditAnywhere, Category = "Lvl2")
	TArray<TSubclassOf<ASTileVariantEnviornment>> OnexOneEnvVariants_lvl2;

	UPROPERTY(EditAnywhere, Category = "Lvl2")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxOneEnvVariants_lvl2;

	UPROPERTY(EditAnywhere, Category = "Lvl2")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxTwoEnvVariants_lvl2;

	UPROPERTY(EditAnywhere, Category = "Lvl2")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexOneEnvVariants_lvl2;

	UPROPERTY(EditAnywhere, Category = "Lvl2")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexTwoEnvVariants_lvl2;

	UPROPERTY(EditAnywhere, Category = "Lvl2")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxTwoEnvVariants_lvl2;

	UPROPERTY(EditAnywhere, Category = "Lvl2")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxThreeEnvVariants_lvl2;

	UPROPERTY(EditAnywhere, Category = "Lvl2")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxFourEnvVariants_lvl2;

	//level 3

	UPROPERTY(EditAnywhere, Category = "Lvl3")
	TArray<TSubclassOf<ASTileVariantEnviornment>> OnexOneEnvVariants_lvl3;

	UPROPERTY(EditAnywhere, Category = "Lvl3")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxOneEnvVariants_lvl3;

	UPROPERTY(EditAnywhere, Category = "Lvl3")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxTwoEnvVariants_lvl3;

	UPROPERTY(EditAnywhere, Category = "Lvl3")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexOneEnvVariants_lvl3;

	UPROPERTY(EditAnywhere, Category = "Lvl3")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexTwoEnvVariants_lvl3;

	UPROPERTY(EditAnywhere, Category = "Lvl3")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxTwoEnvVariants_lvl3;

	UPROPERTY(EditAnywhere, Category = "Lvl3")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxThreeEnvVariants_lvl3;

	UPROPERTY(EditAnywhere, Category = "Lvl3")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxFourEnvVariants_lvl3;

	//level 4

	UPROPERTY(EditAnywhere, Category = "Lvl4")
	TArray<TSubclassOf<ASTileVariantEnviornment>> OnexOneEnvVariants_lvl4;

	UPROPERTY(EditAnywhere, Category = "Lvl4")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxOneEnvVariants_lvl4;

	UPROPERTY(EditAnywhere, Category = "Lvl4")
	TArray<TSubclassOf<ASTileVariantEnviornment>> TwoxTwoEnvVariants_lvl4;

	UPROPERTY(EditAnywhere, Category = "Lvl4")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexOneEnvVariants_lvl4;

	UPROPERTY(EditAnywhere, Category = "Lvl4")
	TArray<TSubclassOf<ASTileVariantEnviornment>> ThreexTwoEnvVariants_lvl4;

	UPROPERTY(EditAnywhere, Category = "Lvl4")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxTwoEnvVariants_lvl4;

	UPROPERTY(EditAnywhere, Category = "Lvl4")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxThreeEnvVariants_lvl4;

	UPROPERTY(EditAnywhere, Category = "Lvl4")
	TArray<TSubclassOf<ASTileVariantEnviornment>> FourxFourEnvVariants_lvl4;
};
