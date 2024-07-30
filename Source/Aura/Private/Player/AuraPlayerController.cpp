// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Input/AuraInputComponent.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates=true;

	Spline=CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaSeconds)
{
	Super::PlayerTick(DeltaSeconds);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if(!CursorHit.bBlockingHit) return;

	LastActor=ThisActor;
	ThisActor=Cast<IAuraInterface>(CursorHit.GetActor());

	/*
	 *	A LastActor is null && ThisActor is null
	 *		- Do nothing
	 *	B LastActor is null && ThisActor is not null
	 *		- Call HighlightActor on ThisActor
	 *	C LastActor is not null && ThisActor is null
	 *		 - Call UnhighlightActor on LastActor
	 *	D LastActor is not null && ThisActor is not null
	 *		 - Call UnhighlightActor on LastActor and HighlightActor on ThisActor
	 *	E LastActor is not null && ThisActor is not null && LastActor is equal to ThisActor
	 *		 - Do nothing
	 */
	if(LastActor==nullptr)
	{
		if(ThisActor!=nullptr)
		{
			// B
			ThisActor->HighlightActor();
		}
		else
		{
			// A Do nothing
		}
	}
	else
	{
		if(ThisActor==nullptr)
		{
			// C
			LastActor->UnhighlightActor();
		}
		else
		{
			if(LastActor!=ThisActor)
			{
				// D
				LastActor->UnhighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// E Do nothing
			}
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting= ThisActor ? true :false;
		bAutoRunning=false;
	}
	GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if(GetASC()==nullptr) return;
	GetASC()->AbilityInputTagReleased(InputTag);

	GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	
	if(bTargeting)
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime+=GetWorld()->GetDeltaSeconds();
	
		FHitResult Hit;
		if(GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			CachedDestination=Hit.ImpactPoint;
		}
	
		if(APawn* ControlledPawn=GetPawn())
		{
			const FVector WorldDirection=(CachedDestination-ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}

	GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Green, *InputTag.ToString());;
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if(AuraAbilitySystemComponent==nullptr)
	{
		AuraAbilitySystemComponent=Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}


void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	

	bShowMouseCursor=true;
	DefaultMouseCursor=EMouseCursor::Default; 

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent=CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue) 
{
	const FVector2d InputAxisVector=InputActionValue.Get<FVector2D>();
	const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);

	const FVector ForwardDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection=FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if(APawn* ControlledPawn=GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
	
}
