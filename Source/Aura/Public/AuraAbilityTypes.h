#pragma once
#include "GameplayEffectTypes.h"


USTRUCT(BlueprintType)
struct FAuraAbilityTypes : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	bool IsCriticalHit() const {return bIsCriticalHit;}
	bool IsBlockedHit() const {return bIsBlockedHit;}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FGameplayEffectContext::StaticStruct();
	}

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

protected:

	UPROPERTY()
	bool bIsBlockedHit=false;

	UPROPERTY()
	bool bIsCriticalHit=false;
};
