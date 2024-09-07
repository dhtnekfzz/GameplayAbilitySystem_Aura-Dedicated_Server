// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride);

protected:

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float ProjectileSpread=90.f;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	int32 MaxNumProjectiles=5;

};
