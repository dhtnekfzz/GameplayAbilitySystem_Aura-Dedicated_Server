// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */

struct FAuraGameplayTags
{
public:
 static const FAuraGameplayTags& Get() { return GameplayTags; }
 static void InitializeNativeGameplayTags();
 
private:
 static FAuraGameplayTags GameplayTags;
};