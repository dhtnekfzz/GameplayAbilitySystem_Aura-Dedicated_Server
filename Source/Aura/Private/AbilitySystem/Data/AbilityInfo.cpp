// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound)
{
	for(const FAuraAbilityInfo& Info : AbilityInformation )
	{
		if(Info.AbilityTag==AbilityTag)
		{
			return Info;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("AbilityInfo for Tag %s not found!"), *AbilityTag.ToString());
	}

	return FAuraAbilityInfo();
}
