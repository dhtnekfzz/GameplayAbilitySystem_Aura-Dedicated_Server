#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject=nullptr;

	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass=nullptr;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent;

	float BaseDamage=0.f;

	float AbilityLevel=1.f;

	FGameplayTag DamageType=FGameplayTag();

	float DebuffChance=0.f;

	float DebuffDamage=0.f;

	float DebuffDuration=0.f;

	float DebuffFrequency=0.f;

	float DeathImpulseMagnitude=0.f;

	FVector DeathImpulse=FVector::ZeroVector;
	
};


USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:

	bool IsCriticalHit() const {return bIsCriticalHit;}
	bool IsBlockedHit() const {return bIsBlockedHit;}
	bool IsSuccessfulDebuff() const {return bIsSuccessfulDebuff;}

	float GetDebuffDamage() const {return DebuffDamage;}
	float GetDebuffDuration() const {return DebuffDuration;}
	float GetDebuffFrequency() const {return DebuffFrequency;}
	TSharedPtr<FGameplayTag> GetDamageType() const {return DamageType;}
	FVector GetDeathImulse() const {return DeathImpulse;}

	void SetIsCriticalHit(bool bInIsCriticalHit) {bIsCriticalHit=bInIsCriticalHit;}
	void SetIsBlockedHit(bool bInIsBlockedHit) {bIsBlockedHit=bInIsBlockedHit;}
	void SetIsSuccessfulDebuff(bool bInIsSuccessfulDebuff) {bIsSuccessfulDebuff=bInIsSuccessfulDebuff;}
	void SetDebuffDamage(float InDebuffDamage) {DebuffDamage=InDebuffDamage;}
	void SetDebuffDuration(float InDebuffDuration) {DebuffDuration=InDebuffDuration;}
	void SetDebuffFrequency(float InDebuffFrequency) {DebuffFrequency=InDebuffFrequency;}
	void SetDamageType(TSharedPtr<FGameplayTag> InDamageType) {DamageType=InDamageType;}
	void SetDeathImpulse(FVector InDeathImpulse) {DeathImpulse=InDeathImpulse;}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}

	virtual FAuraGameplayEffectContext* Duplicate() const override
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:

	UPROPERTY()
	bool bIsBlockedHit=false;

	UPROPERTY()
	bool bIsCriticalHit=false;

	UPROPERTY()
	bool bIsSuccessfulDebuff=false;

	UPROPERTY()
	float DebuffDamage=0.f;

	UPROPERTY()
	float DebuffDuration=0.f;

	UPROPERTY()
	float DebuffFrequency=0.f;

	TSharedPtr<FGameplayTag> DamageType;

	UPROPERTY()
	float DeathImpulseMagnitude=0.f;

	UPROPERTY()
	FVector DeathImpulse=FVector::ZeroVector;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer=true,
		WithCopy=true
	};
};
