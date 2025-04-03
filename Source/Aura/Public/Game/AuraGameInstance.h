// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "AuraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
    void FindSession();

	UFUNCTION(BlueprintCallable)
    void JoinSession();
		
	void StartSession();

public:
	UPROPERTY()
	FName PlayerStartTag=FName();

	UPROPERTY()
	FString LoadSlotName=FString();

	UPROPERTY()
	int32 LoadSlotIndex=0;


private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	void OnFindSessionsComplete(bool Success);
};


