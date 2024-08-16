// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "AuraActor.generated.h"

class USphereComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};

USTRUCT(BlueprintType)
struct FEffectType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	EEffectApplicationPolicy EffectApplicationPolicy=EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	EEffectRemovalPolicy EffectRemovalPolicy=EEffectRemovalPolicy::DoNotRemove;
};


UCLASS()
class AURA_API AAuraActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraActor();


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectorToTarget(AActor* TargetActor, FEffectType& EffectType /*TSubclassOf<UGameplayEffect> GameplayEffectClass*/);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	bool bDestroyOnEffectApplication=false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	bool bApplyEffectsToEnemies=false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	TArray<FEffectType> EffectTypes;
	
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	// TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
 
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	// EEffectApplicationPolicy InstantEffectApplicationPolicy=EEffectApplicationPolicy::DoNotApply;
 
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	// TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	// EEffectApplicationPolicy DurationEffectApplicationPolicy=EEffectApplicationPolicy::DoNotApply;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	// TSubclassOf<UGameplayEffect> InfinityGameplayEffectClass;

	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	// EEffectApplicationPolicy InfinityEffectApplicationPolicy=EEffectApplicationPolicy::DoNotApply;
 
	// UPROPERTY(EditAnywhere, Blueprintable, Category="Applied Effects")
	// EEffectRemovalPolicy InfinityEffectRemovalPolicy=EEffectRemovalPolicy::RemoveOnEndOverlap;

	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent* > ActiveEffectHandles;

	UPROPERTY(EditAnywhere, Blueprintable, Category="Applied Effects")
	float ActorLevel=1.f;
};
