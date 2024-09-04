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
	
};


USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
	
public:

	bool IsCriticalHit() const {return bIsCriticalHit;}
	bool IsBlockedHit() const {return bIsBlockedHit;}

	void SetIsCriticalHit(bool bInIsCriticalHit) {bIsCriticalHit=bInIsCriticalHit;}
	void SetIsBlockedHit(bool bInIsBlockedHit) {bIsBlockedHit=bInIsBlockedHit;}
	
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
