// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"


// Sets default values
AAuraActor::AAuraActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	
}



// Called when the game starts or when spawned
void AAuraActor::BeginPlay()
{
	Super::BeginPlay();

}

void AAuraActor::ApplyEffectorToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(!TargetASC) return;

	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContextHandle= TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle= TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	
}

void AAuraActor::OnOverlap(AActor* TargetActor)
{
	
}

void AAuraActor::OnEndOverlap(AActor* TargetActor)
{
	
}



