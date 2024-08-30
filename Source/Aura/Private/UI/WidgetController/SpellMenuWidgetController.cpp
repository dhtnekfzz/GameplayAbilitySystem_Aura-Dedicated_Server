// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChanged.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag)
	{
		if(SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status=StatusTag;
			bool bEnableSpellPointsButton=false;
			bool bEnableEquipButton=false;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpellPointsButton, bEnableEquipButton);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpellPointsButton, bEnableEquipButton);
		}
		
		if(AbilityInfo)
		{
			FAuraAbilityInfo Info=AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag=StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
	{
		SpellPointsChanged.Broadcast(SpellPoints);
		CurrentSpellPoints=SpellPoints;
	
		bool bEnableSpellPointsButton=false;
		bool bEnableEquipButton=false;
		ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpellPointsButton, bEnableEquipButton);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpellPointsButton, bEnableEquipButton);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FAuraGameplayTags GameplayTags=FAuraGameplayTags::Get();
	const FGameplayAbilitySpec* AbilitySpec=GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const int32 SpellPoints=GetAuraPS()->GetSpellPoints();
	FGameplayTag AbilityStatus;
	
	const bool bTagValid=AbilityTag.IsValid();
	const bool bTagNone=AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	const bool bSpecValid=AbilitySpec!=nullptr;
	if(!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus=GameplayTags.Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus=GetAuraASC()->GetStatusFromSpec(*AbilitySpec);
	}
	
	SelectedAbility.Ability=AbilityTag;
	SelectedAbility.Status=AbilityStatus;
	bool bEnableSpellPointsButton=false;
	bool bEnableEquipButton=false;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpellPointsButton, bEnableEquipButton);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpellPointsButton, bEnableEquipButton);
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
	bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags=FAuraGameplayTags::Get();
	
	bShouldEnableSpendPointsButton=false;
	bShouldEnableEquipButton=false;
	
	if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton=true;
		if(SpellPoints>0) bShouldEnableSpendPointsButton=true;
		
	}
	else if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		if(SpellPoints>0) bShouldEnableSpendPointsButton=true;
	}
	else if(AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton=true;
		if(SpellPoints>0) bShouldEnableSpendPointsButton=true;
	}

}
