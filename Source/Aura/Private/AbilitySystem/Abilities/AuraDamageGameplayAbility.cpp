// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle DamageSpecHandle=MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);

	for(TTuple<FGameplayTag, FScalableFloat> Pair: DamageTypes)
	{
		const float ScaledDamage=Pair.Value.GetValueAtLevel(GetAbilityLevel());
		DamageSpecHandle.Data.Get()->SetSetByCallerMagnitude(Pair.Key, ScaledDamage);
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if(TaggedMontages.IsEmpty()) return FTaggedMontage();
	const int32 Selection=FMath::RandRange(0, TaggedMontages.Num()-1);
	return TaggedMontages[Selection];
	
}

float UAuraDamageGameplayAbility::GetDamageByDamageType(float InLevel, const FGameplayTag& DamageType) const
{
	check(DamageTypes.Contains(DamageType));
	return DamageTypes[DamageType].GetValueAtLevel(InLevel);
}

