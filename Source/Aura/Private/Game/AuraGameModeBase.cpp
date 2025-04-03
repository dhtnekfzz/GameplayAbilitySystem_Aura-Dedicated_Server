// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Aura/AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"
#include "OnlineSubSystem.h"
#include "OnlineSessionSettings.h"
#include "Game/AuraLobbyGameModeBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "GameFramework/Character.h"


void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	if(UGameplayStatics::DoesSaveGameExist(LoadSlot->GetLoadSlotName(), SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(LoadSlot->GetLoadSlotName(), SlotIndex);
	}
	USaveGame* SaveGameObject=UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame=Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName=LoadSlot->GetPlayerName();
	LoadScreenSaveGame->MapName=LoadSlot->GetMapName();
	LoadScreenSaveGame->MapAssetName=LoadSlot->MapAssetName;
	LoadScreenSaveGame->SaveSlotStatus=Taken;
	LoadScreenSaveGame->PlayerStartTag=LoadSlot->PlayerStartTag;
	

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
		
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, const int32 SlotIndex) const
{
	USaveGame* SaveGameObject=nullptr;
	if(UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject=UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject=UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	ULoadScreenSaveGame* LoadScreenSaveGame=Cast<ULoadScreenSaveGame>(SaveGameObject);
	return LoadScreenSaveGame;
	
}

void AAuraGameModeBase::DeleteSlotData(const FString& SlotName, int32 SlotIndex)
{
	if(UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData()
{
	UAuraGameInstance* AuraGameInstance=Cast<UAuraGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName=AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex=AuraGameInstance->LoadSlotIndex;

	return GetSaveSlotData(InGameLoadSlotName,InGameLoadSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveObject)
{
	UAuraGameInstance* AuraGameInstance=Cast<UAuraGameInstance>(GetGameInstance());
	
	const FString InGameLoadSlotName=AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex=AuraGameInstance->LoadSlotIndex;
	AuraGameInstance->PlayerStartTag=SaveObject->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(SaveObject, InGameLoadSlotName, InGameLoadSlotIndex);
}

void AAuraGameModeBase::SaveWorldState(UWorld* World,const FString& DestinationMapAssetName) const
{
	FString WorldName=World->GetName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGI=Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);

	if(ULoadScreenSaveGame* SaveGame=GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
	{
		if(DestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName=DestinationMapAssetName;
			SaveGame->MapName=GetMapNameFromMapAssetName(DestinationMapAssetName);
		}
		
		if(!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSaveMap;
			NewSaveMap.MapAssetName=WorldName;
			SaveGame->SavedMaps.Add(NewSaveMap);
		}

		FSavedMap SavedMap =SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty();
		
		for(FActorIterator It(World); It; ++It)
		{
			AActor* Actor=*It;

			if(!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;
			
			FSavedActor SavedActor;
			SavedActor.ActorName=Actor->GetFName();
			SavedActor.Transform=Actor->GetActorTransform();

			FMemoryWriter MemoryWriter(SavedActor.Bytes);

			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame=true;

			Actor->Serialize(Archive);

			SavedMap.SavedActors.Add(SavedActor);
		}

		for(FSavedMap& MapToReplace: SaveGame->SavedMaps)
		{
			if(MapToReplace.MapAssetName==WorldName)
			{
				MapToReplace=SavedMap;
			}
		}
		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
	}
	
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName=World->GetName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGI=Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);

	if(UGameplayStatics::DoesSaveGameExist(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
	{
		ULoadScreenSaveGame* SaveGame=Cast<ULoadScreenSaveGame>(UGameplayStatics::LoadGameFromSlot(AuraGI->LoadSlotName,AuraGI->LoadSlotIndex));
		if(SaveGame==nullptr)
		{
			UE_LOG(LogAura, Error, TEXT("Failted to load save game"));
		}

		for(FActorIterator It(World); It; ++It)
		{
			AActor* Actor =*It;

			if(!Actor->Implements<USaveInterface>()) continue;

			for(FSavedActor SavedActor : SaveGame->GetSavedMapWithMapName(WorldName).SavedActors)
			{
				if(SavedActor.ActorName==Actor->GetFName())
				{
					if(ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.Transform);
					}
					FMemoryReader MemoryReader(SavedActor.Bytes);

					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame=true;
					Actor->Serialize(Archive);

					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* Slot)
{
	const FString SlotName=Slot->GetLoadSlotName();
	const int32 SlotIndex=Slot->SlotIndex;

	UGameplayStatics::OpenLevelBySoftObjectPtr(Slot, Maps.FindChecked(Slot->GetMapName()));
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for(auto& Map : Maps)
	{
		if(Map.Value.ToSoftObjectPath().GetAssetName()==MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance=Cast<UAuraGameInstance>(GetGameInstance());
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Actors);
	if(Actors.Num()>0)
	{
		AActor* SelectedActor=Actors[0];
		for(AActor* Actor: Actors)
		{
			if(APlayerStart* PlayerStart=Cast<APlayerStart>(Actor))
			{
				if(PlayerStart->PlayerStartTag==AuraGameInstance->PlayerStartTag)
				{
					SelectedActor=PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
	
}

void AAuraGameModeBase::PlayerDied(ACharacter* DeadCharacter)
{
	ULoadScreenSaveGame* SaveGame=RetrieveInGameSaveData();
	if(!IsValid(SaveGame)) return;

	UGameplayStatics::OpenLevel(DeadCharacter, FName(SaveGame->MapAssetName));
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(DefaultMapName, DefaultMap);

	GEngine->SetMaxFPS(30);
	if (IsRunningDedicatedServer())
	{
		CreateSession();
	}
}

void AAuraGameModeBase::CreateSession()
{
	if (IOnlineSubsystem* Subsystem=IOnlineSubsystem::Get())
	{
		SessionInterface= Subsystem->GetSessionInterface();
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &AAuraGameModeBase::OnDestroySessionComplete);

		if(SessionInterface->GetNamedSession(FName("DungeonSession"))!=nullptr) return;
		if (SessionInterface.IsValid())
		{
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsLANMatch = true;  
			SessionSettings.bUsesPresence = false;
			SessionSettings.NumPublicConnections = 4;  
			SessionSettings.bAllowJoinInProgress = true;
			SessionSettings.bShouldAdvertise = true;  
			SessionSettings.bAllowInvites = true;
			SessionSettings.bIsDedicated = true; 
			SessionSettings.bUseLobbiesIfAvailable = true;

			bool bSeesionCreated=SessionInterface->CreateSession(0, FName("DungeonSession"), SessionSettings);

			if (bSeesionCreated)
			{
				UE_LOG(LogTemp, Warning, TEXT("Create Session."));
			}
		}
	}
}

void AAuraGameModeBase::DestroySession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->DestroySession("DungeonSession");
	}
}

void AAuraGameModeBase::OnDestroySessionComplete(FName SessionName,bool Success)
{
	if (Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Destroy Session."));
		CreateSession();
	}
}

void AAuraGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if(!HasAuthority()) return;
	--NumberOfPlayers;

	if (NumberOfPlayers <= 0)
		DestroySession();
		
}

void AAuraGameModeBase::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);

	if(!HasAuthority()) return;
	++NumberOfPlayers;

	if (NumberOfPlayers > 1)
	{
		if(SessionInterface.IsValid()&&SessionInterface->GetSessionState("DungeonSession")==EOnlineSessionState::InProgress) return;
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &AAuraGameModeBase::StartGame, 5);
	}
	
}

void AAuraGameModeBase::StartGame() 
{
	auto GameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (GameInstance == nullptr) return;
	GameInstance->StartSession();
	
	UWorld* World = GameInstance->GetWorld();
	World->ServerTravel("/Game/Maps/Dungeon?listen", ETravelType::TRAVEL_Absolute);
}


void AAuraGameModeBase::EnterDungeon()
{
	GetWorld()->ServerTravel("/Script/Engine.World'/Game/Maps/Dungeon.Dungeon");
}
