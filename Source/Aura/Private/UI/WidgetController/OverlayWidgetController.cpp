// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel, bool bLevelUp)
	{
		OnPlayerLevelChangedDelegate.Broadcast(NewLevel, bLevelUp);
	});

	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);
	

	if(GetAuraASC())
	{
		GetAuraASC()->AbilityEquipped.AddUObject(this, &UOverlayWidgetController::OnAbilityEquipped);
		if(GetAuraASC()->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetAuraASC()->AbilitiesGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}
		
		Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
			{
				for(const FGameplayTag& Tag : AssetTags)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if(Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row=GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}


}


void UOverlayWidgetController::OnXPChanged(int32 NewXP)
{
	const ULevelUpInfo* LevelUpInfo=GetAuraPS()->LevelUpInfo;

	checkf(LevelUpInfo, TEXT("Unabled to find LevelUpInfo. Please fill out PlayerState Blueprint"));

	const int32 Level=LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel=LevelUpInfo->LevelUpInformation.Num();

	if(Level<=MaxLevel && Level>0)
	{
		const int32 LevelUpRequirement=LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelRequirement=LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement;

		const int32 DeltaLevelRequirement=LevelUpRequirement-PreviousLevelRequirement;
		const int32 XPForThisLevel=NewXP-PreviousLevelRequirement;

		const float XPBarPercent=static_cast<float>(XPForThisLevel)/static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status,
	const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	
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
}

