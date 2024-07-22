// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/AuraInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IAuraInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	// Enemy interface
	virtual void HighlightActor() override;
	virtual void UnhighlightActor() override;
	// end Enemy interface

	/* Combat Interface */
	virtual int32 GetPlayerLevel() override;
	/* end Combat Interface */

protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level=1;
};
