// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"


// Sets default values
AAuraActor::AAuraActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	
}



// Called when the game starts or when spawned
void AAuraActor::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraActor::ApplyEffectorToTarget(AActor* TargetActor, FEffectType& EffectType /*TSubclassOf<UGameplayEffect> GameplayEffectClass*/)
{
	if(TargetActor->ActorHasTag(FName("Enemy"))&&!bApplyEffectsToEnemies) return;
	
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(!TargetASC) return;

	check(EffectType.GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle= TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle= TargetASC->MakeOutgoingSpec(EffectType.GameplayEffectClass/*GameplayEffectClass*/, ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle =TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	const bool bIsInfinite=EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy==EGameplayEffectDurationType::Infinite;
	if(bIsInfinite&& EffectType.EffectRemovalPolicy==EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	if(!bIsInfinite)
	{
		Destroy();
	}
}

void AAuraActor::OnOverlap(AActor* TargetActor)
{
	for(auto& Effect : EffectTypes)
	{
		if(Effect.EffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnOverlap)
		{
			ApplyEffectorToTarget(TargetActor, Effect);
		}
	}
	
	// if(InstantEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnOverlap)
	// {
	// 	ApplyEffectorToTarget(TargetActor, InstantGameplayEffectClass);
	// }
	// if(DurationEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnOverlap)
	// {
	// 	ApplyEffectorToTarget(TargetActor, DurationGameplayEffectClass);
	// }
	// if(InfinityEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnOverlap)
	// {
	// 	ApplyEffectorToTarget(TargetActor, InfinityGameplayEffectClass);
	// }
}


void AAuraActor::OnEndOverlap(AActor* TargetActor)
{
	for(auto& Effect : EffectTypes)
	{
		if(Effect.EffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnEndOverlap)
		{
			ApplyEffectorToTarget(TargetActor, Effect);
		}
		else if(Effect.EffectRemovalPolicy==EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			UAbilitySystemComponent* TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if(!IsValid(TargetASC)) return;
	
			TArray<FActiveGameplayEffectHandle> HandlesToRemove;
			for(TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair :ActiveEffectHandles)
			{
				if(TargetASC==HandlePair.Value)
				{
					TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);
					HandlesToRemove.Add(HandlePair.Key);
				}
			}
			for( FActiveGameplayEffectHandle& Handle : HandlesToRemove)
			{
				ActiveEffectHandles.FindAndRemoveChecked(Handle);
			}
		}
	}
	
	
	// if(InstantEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnEndOverlap)
	// {
	// 	ApplyEffectorToTarget(TargetActor, InstantGameplayEffectClass);
	// }
	// if(DurationEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnEndOverlap)
	// {
	// 	ApplyEffectorToTarget(TargetActor, DurationGameplayEffectClass);
	// }
	// if(InfinityEffectApplicationPolicy==EEffectApplicationPolicy::ApplyOnEndOverlap)
	// {
	// 	ApplyEffectorToTarget(TargetActor, InfinityGameplayEffectClass);
	// }
	// if(InfinityEffectRemovalPolicy==EEffectRemovalPolicy::RemoveOnEndOverlap)
	// {
	// 	UAbilitySystemComponent* TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	// 	if(!IsValid(TargetASC)) return;
	//
	// 	TArray<FActiveGameplayEffectHandle> HandlesToRemove;
	// 	for(TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair :ActiveEffectHandles)
	// 	{
	// 		if(TargetASC==HandlePair.Value)
	// 		{
	// 			TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);
	// 			HandlesToRemove.Add(HandlePair.Key);
	// 		}
	// 	}
	// 	for( FActiveGameplayEffectHandle& Handle : HandlesToRemove)
	// 	{
	// 		ActiveEffectHandles.FindAndRemoveChecked(Handle);
	// 	}
	//
	// }
}



