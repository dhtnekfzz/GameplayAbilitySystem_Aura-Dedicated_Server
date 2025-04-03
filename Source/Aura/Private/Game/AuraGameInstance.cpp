// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameInstance.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubSystem.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

void UAuraGameInstance::Init()
{
	Super::Init();
	if (IOnlineSubsystem* Subsystem=IOnlineSubsystem::Get())
	{
		SessionInterface= Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UAuraGameInstance::OnFindSessionsComplete );
		}
	}
}

void UAuraGameInstance::FindSession()
{
	if (SessionInterface->GetNamedSession(FName("DungeonSession"))!=nullptr) return;
	if (SessionInterface.IsValid())
	{
		SessionSearch=MakeShareable(new FOnlineSessionSearch());
		SessionSearch->bIsLanQuery=true;
		SessionSearch->MaxSearchResults=5;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UAuraGameInstance::JoinSession()
{
	if (SessionInterface.IsValid()&&SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
	{
		SessionInterface->JoinSession(0, FName("DungeonSession"), SessionSearch->SearchResults[0]);
		FString Address;
		if (!SessionInterface->GetResolvedConnectString("DungeonSession",Address)) return;
		APlayerController* PlayerController=GetFirstLocalPlayerController();
		PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}

void UAuraGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession("DungeonSession");
		UE_LOG(LogTemp, Warning, TEXT("StartSession"));
	}
}

void UAuraGameInstance::OnFindSessionsComplete(bool Success)
{
	if (Success && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));
		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *Result.GetSessionIdStr());
		}
		UE_LOG(LogTemp, Warning, TEXT("세션 검색 결과 개수: %d"), SessionSearch->SearchResults.Num());
		JoinSession();
	}
}


