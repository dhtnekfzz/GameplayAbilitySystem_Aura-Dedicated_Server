// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage=DamageTypes[FAuraGameplayTags::Get().Damage_Fire].GetValueAtLevel(Level);
	const float ManaCost=FMath::Abs(GetManaCost(Level));
	const float Cooldown=GetCooldown(Level);
	if(Level==1)
	{
		return FString::Printf(TEXT(
			"<TITLE>FIRE BOLT</>\n\n"

			"<Small>Level: </><Level>%d</>\n"

			"<Small>ManaCost: </><ManaCost>%.1f</>\n"

			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			"<Default>Launches a bolt of fire, exlploding on impact and dealing: </>"

			"<Damage>%d</><Default> fire damage with a chance to burn</>"),

			 Level,ManaCost,Cooldown,Damage);
	}
	else
	{
		return FString::Printf(TEXT(
			"<TITLE>FIRE BOLT</>\n\n"

			"<Small>Level: </><Level>%d</>\n"

			"<Small>ManaCost: </><ManaCost>%.1f</>\n"

			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			"<Default>Launches %d bolts of fire, exlploding on impact and dealing: </>"

			"<Damage>%d</><Default> fire damage with a chance to burn</>"),

			 Level,ManaCost,Cooldown,FMath::Min(Level, NumProjectiles),Damage);
		
	}
	
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage=GetDamageByDamageType(Level,FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost=FMath::Abs(GetManaCost(Level));
	const float Cooldown=GetCooldown(Level);
	return  FString::Printf(TEXT(
		"<TITLE>NEXT LEVEL</>\n\n"

		"<Small>Level: </>""<Level>%d</>\n"

		"<Small>ManaCost: </>""<ManaCost>%.1f</>\n"

		"<Small>Cooldown: </>""<Cooldown>%.1f</>\n\n"
		
		"<Default>Launches %d bolts of fire, exlploding on impact and dealing: </>"
		
		"<Damage>%d</><Default> fire damage with a chance to burn</>"),
		
		Level,ManaCost,Cooldown ,FMath::Min(Level, NumProjectiles),Damage);
}