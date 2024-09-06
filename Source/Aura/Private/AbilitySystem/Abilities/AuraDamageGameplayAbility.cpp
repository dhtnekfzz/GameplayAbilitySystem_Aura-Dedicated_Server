// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FGameplayEffectSpecHandle DamageSpecHandle=MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1);
	
	const float ScaledDamage=Damage.GetValueAtLevel(GetAbilityLevel());
	DamageSpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageType, ScaledDamage);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor)
{
	FDamageEffectParams Params;
	Params.WorldContextObject=GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass=DamageEffectClass;
	Params.SourceAbilitySystemComponent=GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage=Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel=GetAbilityLevel();
	Params.DamageType=DamageType;
	Params.DebuffChance=DebuffChance;
	Params.DebuffDamage=DebuffDamage;
	Params.DebuffDuration=DebuffDuration;
	Params.DebuffFrequency=DebuffFrequency;
	Params.DeathImpulseMagnitude=DeathImpulseMagnitude;
	return Params;
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const
{
	if(TaggedMontages.IsEmpty()) return FTaggedMontage();
	const int32 Selection=FMath::RandRange(0, TaggedMontages.Num()-1);
	return TaggedMontages[Selection];
	
}


