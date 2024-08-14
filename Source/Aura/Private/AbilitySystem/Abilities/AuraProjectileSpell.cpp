// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileLocation)
{
	const bool IsServer=GetAvatarActorFromActorInfo()->HasAuthority();
	if(!IsServer) return;

	ICombatInterface* CombatInterface=Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if(CombatInterface)
	{
		const FVector SocketLocation=CombatInterface->GetCombatSocketLocation();
		FRotator Rotation=(ProjectileLocation-SocketLocation).Rotation();
		Rotation.Pitch=0;

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
		

		AAuraProjectile* Projectile=GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		const UAbilitySystemComponent* SourceASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle=SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location =ProjectileLocation;
		EffectContextHandle.AddHitResult(HitResult);
		
		const FGameplayEffectSpecHandle SpecHandle=SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		const FAuraGameplayTags GameplayTags=FAuraGameplayTags::Get();

		for(auto& Pair : DamageTypes)
		{
			const float ScaledDamage=Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}

		Projectile->DamageEffectSpecHandle=SpecHandle;
		
		Projectile->FinishSpawning(SpawnTransform);
	}
	
}
