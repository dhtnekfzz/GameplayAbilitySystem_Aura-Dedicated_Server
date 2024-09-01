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
	GetAuraASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 Level)
	{
		if(SelectedAbility.Ability.MatchesTagExact(AbilityTag))
		{
			SelectedAbility.Status=StatusTag;
			bool bEnableSpellPointsButton=false;
			bool bEnableEquipButton=false;
			ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpellPointsButton, bEnableEquipButton);
			FString Description;
			FString NextLevelDescription;
			GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpellPointsButton, bEnableEquipButton,Description, NextLevelDescription);
		}
		
		if(AbilityInfo)
		{
			FAuraAbilityInfo Info=AbilityInfo->FindAbilityInfoForTag(AbilityTag);
			Info.StatusTag=StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});

	GetAuraASC()->AbilityEquipped.AddUObject(this, &USpellMenuWidgetController::OnAbilityEquipped);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda([this](int32 SpellPoints)
	{
		SpellPointsChanged.Broadcast(SpellPoints);
		CurrentSpellPoints=SpellPoints;
		bool bEnableSpellPointsButton=false;
		bool bEnableEquipButton=false;
		ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpellPointsButton, bEnableEquipButton);
		FString Description;
		FString NextLevelDescription;
		GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpellPointsButton, bEnableEquipButton,Description, NextLevelDescription);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if(bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType=AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection=false;
	}
	
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
	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionsByAbilityTag(SelectedAbility.Ability, Description, NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpellPointsButton, bEnableEquipButton,Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if(GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::GlobeDeselect()
{
	if(bWaitingForEquipSelection)
	{
		const FGameplayTag SelectedAbilityType=AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
		StopWaitForEquipDelegate.Broadcast(SelectedAbilityType);
		bWaitingForEquipSelection=false;
	}
	SelectedAbility.Ability=FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.Status=FAuraGameplayTags::Get().Abilities_Status_Locked;
	SpellGlobeSelectedDelegate.Broadcast(false, false, FString(), FString());
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag AbilityType=AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	
	WaitForEquipDelegate.Broadcast(AbilityType);
	bWaitingForEquipSelection=true;

	const FGameplayTag SelectedStatus=GetAuraASC()->GetStatusFromAbilityTag(SelectedAbility.Ability);
	if(SelectedStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Equipped))
	{
		SelectedSlot=GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	if(!bWaitingForEquipSelection) return;

	const FGameplayTag& SelectedAbilityType=AbilityInfo->FindAbilityInfoForTag(SelectedAbility.Ability).AbilityType;
	if(!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.Ability,SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	bWaitingForEquipSelection=false;

	const FAuraGameplayTags GameplayTags=FAuraGameplayTags::Get();

	FAuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag=GameplayTags.Abilities_Status_Unlocked;
	LastSlotInfo.InputTag=PreviousSlot;
	LastSlotInfo.AbilityTag=GameplayTags.Abilities_None;
	AbilityInfoDelegate.Broadcast(LastSlotInfo);

	FAuraAbilityInfo Info=AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	Info.StatusTag=Status;
	Info.InputTag=Slot;
	AbilityInfoDelegate.Broadcast(Info);

	StopWaitForEquipDelegate.Broadcast(AbilityInfo->FindAbilityInfoForTag(AbilityTag).AbilityType);

	
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
