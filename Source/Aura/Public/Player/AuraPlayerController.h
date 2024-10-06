// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/AuraInterface.h"
#include "Components/SplineComponent.h"
#include "AuraPlayerController.generated.h"

class IHighlightInterface;
class UNiagaraSystem;
class UDamageTextComponent;
class UAuraAbilitySystemComponent;
class UInputAction;
struct FInputActionValue;
class UInputMappingContext;
class UAuraInputConfig;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaSeconds) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter,bool bIsCriticalHit, bool bIsBlockedHit);
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed(){bShiftKeyDown=true;};
	void ShiftReleased(){bShiftKeyDown=false;};
	bool bShiftKeyDown=false;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();
	
	IHighlightInterface* ThisActor;
	IHighlightInterface* LastActor;
	FHitResult CursorHit;

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;
	
	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination= FVector::ZeroVector;
	float FollowTime=0.f;
	float ShortPressThreshold=0.5;
	bool bAutoRunning=false;
	bool bTargeting=false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius=50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;
	
	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};


