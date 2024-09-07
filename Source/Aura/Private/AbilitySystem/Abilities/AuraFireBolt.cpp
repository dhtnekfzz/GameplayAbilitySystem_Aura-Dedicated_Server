// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage=Damage.GetValueAtLevel(Level);
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

			 Level,ManaCost,Cooldown,ScaledDamage);
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

			 Level,ManaCost,Cooldown,FMath::Min(Level, NumProjectiles),ScaledDamage);
		
	}
	
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage=Damage.GetValueAtLevel(Level);
	const float ManaCost=FMath::Abs(GetManaCost(Level));
	const float Cooldown=GetCooldown(Level);
	return  FString::Printf(TEXT(
		"<TITLE>NEXT LEVEL</>\n\n"

		"<Small>Level: </>""<Level>%d</>\n"

		"<Small>ManaCost: </>""<ManaCost>%.1f</>\n"

		"<Small>Cooldown: </>""<Cooldown>%.1f</>\n\n"
		
		"<Default>Launches %d bolts of fire, exlploding on impact and dealing: </>"
		
		"<Damage>%d</><Default> fire damage with a chance to burn</>"),
		
		Level,ManaCost,Cooldown ,FMath::Min(Level, NumProjectiles),ScaledDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileLocation, const FGameplayTag& SocketTag,
	bool bOverridePitch, float PitchOverride)
{
	const bool IsServer=GetAvatarActorFromActorInfo()->HasAuthority();
	if(!IsServer) return;

	const FVector SocketLocation=ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation=(ProjectileLocation-SocketLocation).Rotation();

	if(bOverridePitch) Rotation.Pitch=PitchOverride;

	const FVector Forward=Rotation.Vector();
	const FVector LeftOfSpread=Forward.RotateAngleAxis(-ProjectileSpread/2.f, FVector::UpVector);
	const FVector RightOfSpread=Forward.RotateAngleAxis(ProjectileSpread/2.f, FVector::UpVector);
	
	//NumProjectiles=FMath::Min(GetAbilityLevel(),MaxNumProjectiles);

	if(NumProjectiles>1)
	{
		const float DeltaSpread=ProjectileSpread/(NumProjectiles-1);
		for(int i=0; i<NumProjectiles; i++)
		{
			const FVector Direction=LeftOfSpread.RotateAngleAxis(DeltaSpread*i, FVector::UpVector);
			
			UKismetSystemLibrary::DrawDebugArrow(
				GetWorld(),
				SocketLocation,
				SocketLocation+Direction*100.f,
				1.f,
				FLinearColor::Red,
				120,
				1
			);
		}
	}
	else
	{
		
	}
}
