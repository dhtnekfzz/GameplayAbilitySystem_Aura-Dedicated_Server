// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;
struct FGameplayTag;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	EnterName,
	Taken
};

USTRUCT()
struct FSavedActor
{
	GENERATED_BODY()

	UPROPERTY()
	FName ActorName=FName();

	UPROPERTY()
	FTransform Transform=FTransform();

	UPROPERTY()
	TArray<uint8> Bytes;
};

USTRUCT()
struct FSavedMap
{
	GENERATED_BODY()

	UPROPERTY()
	FString MapAssetName=FString();

	UPROPERTY()
	TArray<FSavedActor> SavedActors;
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ClassDefaults")
	TSubclassOf<UGameplayAbility> GameplayAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag=FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityStatus=FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilitySlot=FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityType=FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AbilityLevel=1;
	
};

/**
 * 
 */
UCLASS()
class AURA_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	UPROPERTY()
	FString SlotName=FString();

	UPROPERTY()
	int32 SlotIndex=0;

	UPROPERTY()
	FString PlayerName=FString("Default Name");

	UPROPERTY()
	FString MapName=FString("Default Map Name");

	UPROPERTY()
	FString MapAssetName=FString("Default Map Asset Name");

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SaveSlotStatus=Vacant;

	UPROPERTY()
	bool bFirstTimeLoadIn=true;

	/* Player */

	UPROPERTY()
	int32 PlayerLevel=1;

	UPROPERTY()
	int32 XP=0;

	UPROPERTY()
	int32 SpellPoints=0;

	UPROPERTY()
	int32 AttributePoints=0;

	/* Attributes */

	UPROPERTY()
	float Strength=0;
	
	UPROPERTY()
	float Intelligence=0;

	UPROPERTY()
	float Resilience=0;

	UPROPERTY()
	float Vigor;

	/* Abilities */

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;

	UPROPERTY()
	TArray<FSavedMap> SavedMaps;

	FSavedMap GetSavedMapWithMapName(const FString& InMapName);
	bool HasMap(const FString& InMapName);
	
};
