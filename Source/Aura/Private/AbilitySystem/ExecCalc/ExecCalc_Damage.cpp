// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		const FAuraGameplayTags& Tags=FAuraGameplayTags::Get();
		
	}
	
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}



UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
}

void UExecCalc_Damage::DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams, TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs, const FGameplayEffectSpec& Spec, FAggregatorEvaluateParameters EvaluationParameters) const
{
	const FAuraGameplayTags GameplayTags=FAuraGameplayTags::Get();

	for(TTuple<FGameplayTag, FGameplayTag> Pair : GameplayTags.DamageTypesToDebuffs)
	{
		const FGameplayTag& DamageType=Pair.Key;
		const FGameplayTag& DebuffType=Pair.Value;
		const float TypeDamage=Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);

		if(TypeDamage>-.5f)
		{
			const float SourceDebuffChance=Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Chance, false, -1.f);
			
			float TargetDebuffResistance=0.f;
			const FGameplayTag& ResistanceTag=GameplayTags.DamageTypesToResistances[DamageType];
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TagsToCaptureDefs[ResistanceTag], EvaluationParameters, TargetDebuffResistance);
			TargetDebuffResistance=FMath::Max<float>(TargetDebuffResistance, 0.f);
			const float EffectiveDebuffChance=TypeDamage*(100.f-TargetDebuffResistance)/100.f;
			const bool bDebuff=FMath::RandRange(1, 100)<EffectiveDebuffChance;
			if(bDebuff)
			{
				FGameplayEffectContextHandle ContextHandle=Spec.GetContext();

				UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
				UAuraAbilitySystemLibrary::SetDamageType(ContextHandle, DamageType);

				const float DebuffDamage=Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Damage, false, -1.f);
				const float DebuffDuration=Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Duration, false, -1.f);
				const float DebuffFrequency=Spec.GetSetByCallerMagnitude(GameplayTags.Debuff_Frequency, false, -1.f);

				UAuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
				UAuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
				UAuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
			}
		}
		
	}
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	const FAuraGameplayTags& Tags=FAuraGameplayTags::Get();

	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, DamageStatics().ArmorDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, DamageStatics().BlockChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, DamageStatics().ArmorPenetrationDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, DamageStatics().CriticalHitChanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, DamageStatics().CriticalHitResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, DamageStatics().CriticalHitDamageDef);

	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, DamageStatics().ArcaneResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, DamageStatics().FireResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, DamageStatics().LightningResistanceDef);
	TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, DamageStatics().PhysicalResistanceDef);
	
	
	const UAbilitySystemComponent* SourceASC=ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC=ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar=SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar=TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	int32 TargetPlayerLevel=1;
	if(TargetAvatar->Implements<UCombatInterface>())
	{
		TargetPlayerLevel=ICombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

	int32 SourcePlayerLevel=1;
	if(SourceAvatar->Implements<UCombatInterface>())
	{
		SourcePlayerLevel=ICombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}

	const FGameplayEffectSpec& Spec=ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags=Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags=Spec.CapturedTargetTags.GetAggregatedTags();

	//Debuff
	DetermineDebuff(ExecutionParams, TagsToCaptureDefs, Spec, EvaluationParameters);

	// Get Damage Set by Caller Magnitude
	float Damage=0.f;
	for(const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTypeTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;

		checkf(TagsToCaptureDefs.Contains(ResistanceTag), TEXT("Resistance Tag %s not found in TagsToCaptureDefs"), *ResistanceTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition CaptureDef=TagsToCaptureDefs[ResistanceTag];
			
		float DamageTypeValue=Spec.GetSetByCallerMagnitude(Pair.Key);

		float Resistance=0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		Resistance=FMath::Clamp(Resistance, 0.f, 100.f);

		DamageTypeValue*=(100.f-Resistance)/100.f;

		UE_LOG(LogTemp, Warning, TEXT("Resistance: %f"), Resistance);
		
		Damage+=DamageTypeValue;
	}

	float TargetBlockChance=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance=FMath::Max<float>(TargetBlockChance, 0.f);

	const bool bBlocked=FMath::RandRange(1,100) <TargetBlockChance;

	FGameplayEffectContextHandle EffectContextHandle=Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	
	Damage= bBlocked ? Damage/2.f : Damage;

	float TargetArmor=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor=FMath::Max<float>(TargetArmor, 0.f);

	float SourceArmorPenetration=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	TargetArmor=FMath::Max<float>(SourceArmorPenetration, 0.f);

	UCharacterClassInfo* CharacterClassInfo=UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	const FRealCurve* ArmorPenetrationCurve=CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"),FString());
	const float ArmorPenetrationCoefficient=ArmorPenetrationCurve->Eval(SourcePlayerLevel);

	const FRealCurve* EffectiveArmorCurve=CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"),FString());
	const float EffectiveArmorCoefficient=EffectiveArmorCurve->Eval(TargetPlayerLevel);

	const float EffectiveArmor=	TargetArmor*(100-SourceArmorPenetration*ArmorPenetrationCoefficient)/100.f;
	Damage*=(100-EffectiveArmor*EffectiveArmorCoefficient)/100.f;

	float SourceCriticalHitChance=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance=FMath::Max<float>(SourceCriticalHitChance, 0.f);
	
	float TargetCriticalHitResistance=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance=FMath::Max<float>(SourceCriticalHitChance, 0.f);

	float SourceCriticalHitDamage=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage=FMath::Max<float>(SourceCriticalHitChance, 0.f);

	const FRealCurve* CriticalHitResistanceCurve=CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"),FString());
	const float CriticalHitResistanceCoefficient=CriticalHitResistanceCurve->Eval(TargetPlayerLevel);
	
	const float EffectiveCriticalHitChance=SourceCriticalHitChance-TargetCriticalHitResistance*CriticalHitResistanceCoefficient;
	const bool bCriticalHit=FMath::RandRange(1,100) <EffectiveCriticalHitChance;

	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	
	Damage= bCriticalHit ? Damage*2.f+SourceCriticalHitDamage : Damage;

	
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);

}
