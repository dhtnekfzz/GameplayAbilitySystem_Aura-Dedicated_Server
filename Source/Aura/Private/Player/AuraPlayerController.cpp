// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AuraInputComponent.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates=true;
	
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
	
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	
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
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagHeld);
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
