// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AuraGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerState.h"
#include "GameFramework/SpringArmComponent.h"
#include "UI/HUD/AuraHUD.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"


AAuraCharacter::AAuraCharacter()
{
	CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest=false;

	TopDownCameraComponent=CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCameraComponent"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation=false;

	LevelUpNiagaraComponent=CreateDefaultSubobject<UNiagaraComponent>(TEXT("LevelUpNiagaraComponent"));
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate=false;
	
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.0f,400.0f,0.0f);
	GetCharacterMovement()->bConstrainToPlane=true;
	GetCharacterMovement()->bSnapToPlaneAtStart=true;

	bUseControllerRotationPitch=false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationYaw=false;
	
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the Server
	InitAbilityActorInfo();
	LoadProgress();

}

void AAuraCharacter::LoadProgress()
{
	AAuraGameModeBase* AuraGameMode=Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(AuraGameMode)
	{
		ULoadScreenSaveGame* SaveData=AuraGameMode->RetrieveInGameSaveData();
		if(SaveData==nullptr) return;

		
		if(SaveData->bFirstTimeLoadIn)
		{
			InitializeDefaultAttributes();
			AddCharacterAbilities();
		}
		else
		{
			if(UAuraAbilitySystemComponent* AuraASC=Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
			{
				AuraASC->AddCharacterAbilitiesFromSaveData(SaveData);
			}
			
			if(AAuraPlayerState* AuraPlayerState=Cast<AAuraPlayerState>(GetPlayerState()))
			{
				AuraPlayerState->SetLevel(SaveData->PlayerLevel);
				AuraPlayerState->SetXP(SaveData->XP);
				AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
				AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
			}

			UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData);
		}
		
	}
}


void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the Client
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToXP(InXP);
	
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	const AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	const AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	const AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel) const
{
	AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(InPlayerLevel);

	if(UAuraAbilitySystemComponent* AuraASC=Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatues(AuraPlayerState->GetPlayerLevel());
	}
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	AAuraGameModeBase* AuraGameMode=Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if(AuraGameMode)
	{
		ULoadScreenSaveGame* SaveData=AuraGameMode->RetrieveInGameSaveData();
		if(SaveData==nullptr) return;

		SaveData->PlayerStartTag=CheckpointTag;

		if(AAuraPlayerState* AuraPlayerState=Cast<AAuraPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel=AuraPlayerState->GetPlayerLevel();
			SaveData->XP=AuraPlayerState->GetXP();
			SaveData->SpellPoints=AuraPlayerState->GetSpellPoints();
			SaveData->AttributePoints=AuraPlayerState->GetAttributePoints();
		}
		SaveData->Strength=UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence=UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience=UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor=UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());
		SaveData->bFirstTimeLoadIn=false;

		if(!HasAuthority()) return;

		UAuraAbilitySystemComponent* AuraASC=Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
		FForEachAbility SaveAbilityDelegate;
		SaveData->SavedAbilities.Empty();
		SaveAbilityDelegate.BindLambda([this, AuraASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
		{
			const FGameplayTag AbilityTag=AuraASC->GetAbilityTagFromSpec(AbilitySpec);
			UAbilityInfo* AbilityInfo=UAuraAbilitySystemLibrary::GetAbilityInfo(this);
			FAuraAbilityInfo Info=AbilityInfo->FindAbilityInfoForTag(AbilityTag);

			FSavedAbility SavedAbility;
			SavedAbility.GameplayAbility=Info.Ability;
			SavedAbility.AbilityLevel=AbilitySpec.Level;
			SavedAbility.AbilitySlot=AuraASC->GetSlotTagFromAbilityTag(AbilityTag);
			SavedAbility.AbilityStatus=AuraASC->GetStatusFromAbilityTag(AbilityTag);
			SavedAbility.AbilityTag=AbilityTag;
			SavedAbility.AbilityType=Info.AbilityType;

			SaveData->SavedAbilities.Add(SavedAbility);
			
		});
		AuraASC->ForEachAbility(SaveAbilityDelegate);

		AuraGameMode->SaveInGameProgressData(SaveData);
	}
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	const AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::OnRep_Stunned()
{
	if(UAuraAbilitySystemComponent* AuraASC=Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		const FAuraGameplayTags& GameplayTags=FAuraGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
		if(bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
		
	}
}

void AAuraCharacter::OnRep_Burned()
{
	if(bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	AbilitySystemComponent=AuraPlayerState->GetAbilitySystemComponent();
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	AttributeSet=AuraPlayerState->GetAttributeSet();
	OnAscRegistered.Broadcast(AbilitySystemComponent);

	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&AAuraCharacter::StunTagChanged
	);

	if(APlayerController* AuraPlayerController=Cast<APlayerController>(GetController()))
	{
		if(AAuraHUD* AuraHUD=Cast<AAuraHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	
	InitializeDefaultAttributes();
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if(IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation=TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation=LevelUpNiagaraComponent->GetComponentLocation();
	    const FRotator ToCameraRotation=(CameraLocation-NiagaraSystemLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		LevelUpNiagaraComponent->Activate(true);
	}
}