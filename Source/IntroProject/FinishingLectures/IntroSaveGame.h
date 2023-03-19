// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "IntroSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	UPROPERTY(visibleAnywhere, Category = "SaveGameData")
	FString WeaponName;

	UPROPERTY(visibleAnywhere, Category = "SaveGameData")
	FString LevelName;
};

/**
 * 
 */
UCLASS()
class INTROPROJECT_API UIntroSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	public:
	UIntroSaveGame();

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	int32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = "Basic")
	FCharacterStats CharacterStats;
};
