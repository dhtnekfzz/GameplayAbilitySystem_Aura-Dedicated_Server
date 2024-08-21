// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocation();

	UFUNCTION(BlueprintPure, Category="Sumoning")
	TSubclassOf<APawn> GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly, Category="Sumoning")
	int32 NumMinions=5;

	UPROPERTY(EditDefaultsOnly, Category="Sumoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category="Sumoning")
	float MinSpawnDistance=50.f;

	UPROPERTY(EditDefaultsOnly, Category="Sumoning")
	float MaxSpawnDistance=250.f;

	UPROPERTY(EditDefaultsOnly, Category="Sumoning")
	float SpawnSpread=90.f;
	
};
