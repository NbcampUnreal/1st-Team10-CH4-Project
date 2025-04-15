#include "GameInstance/CSGameInstance.h"
#include "Controller/CSPlayerController.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSubsystemUtils.h"
#include "Data/CSLevelRow.h"
#include "Data/CSCharacterRow.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/NetDriver.h" 

UCSGameInstance::UCSGameInstance() :
	SessionInterface(nullptr),
	SessionSearch(nullptr),
	MatchType(EMatchType::EMT_MainMenu), 
	ExpectedPlayerCount(0),
	CharacterData(nullptr),
	AIData(nullptr),
	LevelData(nullptr),
	bIsSessionCreated(false),
	bAutoHostIfNoSession(false)
{
}

void UCSGameInstance::Init()
{
	Super::Init();

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem) 
	{
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) 
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCSGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCSGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCSGameInstance::OnJoinSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UCSGameInstance::OnDestroySessionComplete);
		}
	}
}

void UCSGameInstance::Shutdown()
{
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
				FOnDestroySessionCompleteDelegate::CreateUObject(this, &UCSGameInstance::OnDestroySessionComplete)
			);
			if (!SessionInterface->DestroySession(NAME_GameSession))
			{
				SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
			}
		}
	}
	Super::Shutdown(); 
}

void UCSGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
}

void UCSGameInstance::SetMatchType(EMatchType NewType)
{
	MatchType = NewType;
}

void UCSGameInstance::HostSession(EMatchType TypeToHost)
{
	if (!SessionInterface.IsValid()) return;

	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
			FOnDestroySessionCompleteDelegate::CreateWeakLambda(this, [this, TypeToHost](FName, bool bWasSuccessful) {
				SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
				if (bWasSuccessful) 
				{
					HostSession(TypeToHost); 
				}
				})
		);

		if (!SessionInterface->DestroySession(NAME_GameSession))
		{
			SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		}
		return;
	}

	SetMatchType(TypeToHost);

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = false; //SteamOSS
	SessionSettings.NumPublicConnections = 4;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bUseLobbiesIfAvailable = true; //SteamOSS
	SessionSettings.Set(FName("BUILDUNIQUEID"), 1, EOnlineDataAdvertisementType::ViaOnlineService);

	FUniqueNetIdRepl ReplNetId = GetFirstGamePlayer()->GetPreferredUniqueNetId();
	TSharedPtr<const FUniqueNetId> UserId = ReplNetId.GetUniqueNetId();

	if (!UserId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("HostSession: Invalid UniqueNetId"));
		return;
	}

	SessionInterface->CreateSession(*UserId, NAME_GameSession, SessionSettings);
}

void UCSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("Session created: %s | Success: %d"), *SessionName.ToString(), bWasSuccessful);

	if (bWasSuccessful)
	{
		SessionInterface->StartSession(NAME_GameSession);

		// 로비로 이동
		FTimerHandle DelayTravelHandle;
		GetWorld()->GetTimerManager().SetTimer(DelayTravelHandle, FTimerDelegate::CreateLambda([this]()
			{
				FString TravelURL = TEXT("/Game/Blueprints/Hud/Maps/LobbyLevel?listen");
				GetWorld()->ServerTravel(TravelURL);
			}), 2.0f, false);
	}
}

void UCSGameInstance::FindSessions()
{
	/*UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Finding sessions...")), true, false, FLinearColor(1.0, 0.0, 0.0, 1), 30.0f);*/

	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false; //SteamOSS
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); //SteamOSS

	FUniqueNetIdRepl ReplNetId = GetFirstGamePlayer()->GetPreferredUniqueNetId();
	TSharedPtr<const FUniqueNetId> UserId = ReplNetId.GetUniqueNetId();

	if (!UserId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("FindSessions: Invalid UniqueNetId"));
		return;
	}

	SessionInterface->FindSessions(*UserId, SessionSearch.ToSharedRef());
}

void UCSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 서치 유효하지 않음"));
		return;
	}

	if (SessionSearch->SearchResults.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 서치 결과 없음"));
	}

	bool bFoundSession = false;

	TSharedPtr<const FUniqueNetId> MyNetId;
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem && Subsystem->GetIdentityInterface().IsValid())
	{
		MyNetId = Subsystem->GetIdentityInterface()->GetUniquePlayerId(0);
	}

	for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
	{
		if (!Result.IsValid()) continue;

		// 🔒 자기 세션 필터링
		if (MyNetId.IsValid() && Result.Session.OwningUserId.IsValid() &&
			*Result.Session.OwningUserId == *MyNetId)
		{
			UE_LOG(LogTemp, Log, TEXT("🔄 Skipping self-hosted session"));
			continue;
		}

		// 🔒 BUILDUNIQUEID 필터링
		int32 BuildId = -1;
		if (!Result.Session.SessionSettings.Get(FName("BUILDUNIQUEID"), BuildId) || BuildId != 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("⛔ Skipping session with incompatible BUILDUNIQUEID: %d"), BuildId);
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("✅ Found valid session! Attempting to join..."));
		JoinFoundSession(Result);
		bFoundSession = true;
		break;
	}

	if (!bFoundSession)
	{
		if (bAutoHostIfNoSession)
		{
			bAutoHostIfNoSession = false;
			UE_LOG(LogTemp, Log, TEXT("🛠 No valid session found. Hosting..."));
			HostSession(MatchType);
		}
		else
		{
			if (ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
			{
				PC->Client_ShowNoSessionPopup();
			}
		}
	}
}

void UCSGameInstance::JoinFoundSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!SessionInterface.IsValid()) return;
	
	SessionInterface->JoinSession(0, NAME_GameSession, SearchResult);
}

void UCSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance - OnJoinSessionComplete: Result=%d"), (int32)Result);
	if (!SessionInterface.IsValid()) return;

	FString ConnectString;
	if (Result == EOnJoinSessionCompleteResult::Success &&
		SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		FString HostIP;
		FString Port;
		if (ConnectString.Split(":", &HostIP, &Port) && Port == "0")
		{
			ConnectString = HostIP + TEXT(":7777");
			UE_LOG(LogTemp, Warning, TEXT("Port was 0. Overriding to 7777: %s"), *ConnectString);
		}

		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to JoinSession."));
	}
}

void UCSGameInstance::StartArcadeMode()
{
	UWorld* World = GetWorld();
	if (!World) return;
	SetMatchType(EMatchType::EMT_Single);

	UGameplayStatics::OpenLevel(World, FName("/Game/Blueprints/Hud/Maps/SingleModeLevel")); // 전체 경로 사용
}

void UCSGameInstance::ServerTravelToLevel(FName LevelID)
{
	UWorld* World = GetWorld();
	if (!LevelData || !World) return;
	const FString ContextStr = TEXT("ServerTravelToLevel");
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, ContextStr)) {
		if (!Row->MapPath.IsEmpty()) 
		{
			FString TravelURL = Row->MapPath + TEXT("?listen");
			World->ServerTravel(TravelURL);
		}
		
	}
}

void UCSGameInstance::OpenLevelByID(FName LevelID)
{
	UWorld* World = GetWorld();
	if (!LevelData || !World) return;
	const FString ContextStr = TEXT("OpenLevelByID");
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, ContextStr)) {
		if (!Row->MapPath.IsEmpty()) 
		{
			UGameplayStatics::OpenLevel(World, FName(*Row->MapPath)); 
		}
	}
}

void UCSGameInstance::ResetLobbySettings() 
{ 
	ExpectedPlayerCount = 0; 
	PlayerLobbyDataMap.Empty();
}

const FCharacterRow* UCSGameInstance::FindCharacterRowByJob(EJobTypes Job) const
{
	if (!CharacterData) return nullptr;
	const UEnum* EnumPtr = StaticEnum<EJobTypes>();
	if (!EnumPtr) return nullptr;

	FString EnumString = EnumPtr->GetNameStringByValue(static_cast<int64>(Job));
	EnumString.RemoveFromStart(TEXT("EJT_"));
	EnumString.TrimStartAndEndInline(); 

	FName RowName = FName(*EnumString);
	
	return CharacterData->FindRow<FCharacterRow>(RowName, TEXT("FindCharacterRowByJob"));
}

const FLevelRow* UCSGameInstance::FindLevelRow(FName RowName) const
{
	if (!LevelData) return nullptr;
	const FString Context = TEXT("FineLevelRow");

	return LevelData->FindRow<FLevelRow>(RowName, Context);
}

void UCSGameInstance::SetPlayerLobbyData(const FString& PlayerName, const FPlayerLobbyData& Data)
{
	PlayerLobbyDataMap.Add(PlayerName, Data);
}

FPlayerLobbyData UCSGameInstance::GetPlayerLobbyData(const FString& PlayerName) const
{
	if (const FPlayerLobbyData* Found = PlayerLobbyDataMap.Find(PlayerName))
	{
		return *Found;
	}
	return FPlayerLobbyData();
}
