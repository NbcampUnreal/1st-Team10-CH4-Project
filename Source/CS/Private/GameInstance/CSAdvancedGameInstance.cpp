#include "GameInstance/CSAdvancedGameInstance.h"
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
#include "AdvancedSessionsLibrary.h"

void UCSAdvancedGameInstance::Init()
{
	Super::Init();

	MatchType = EMatchType::EMT_MainMenu;
	ExpectedPlayerCount = 0;
	bAutoHostIfNoSession = false;
}

void UCSAdvancedGameInstance::SafeJoinSession(const FBlueprintSessionResult& SearchResult)
{
	const FOnlineSessionSearchResult& NativeResult = SearchResult.OnlineResult;
	UE_LOG(LogTemp, Warning, TEXT("SafeJoinSession called"));
	
	const int32 BuildID = NativeResult.Session.SessionSettings.BuildUniqueId;
	const int32 Ping = NativeResult.PingInMs;
	const bool bValidInfo = NativeResult.Session.SessionInfo.IsValid();

	const bool bDefinitelyDead = (BuildID == 0 && Ping == 9999);
	if (bDefinitelyDead || !bValidInfo)
	{
		OnJoinSessionFailed.Broadcast();

		return;
	}

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!PlayerController) return;


	CachedSessionResult = NativeResult;
	JoinSessionDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		FOnJoinSessionCompleteDelegate::CreateUObject(this, &UCSAdvancedGameInstance::OnJoinSessionComplete)
	);

	SessionInterface->JoinSession(0, NAME_GameSession, NativeResult);
}

void UCSAdvancedGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (!SessionInterface.IsValid()) return;

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionDelegateHandle);

	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		OnJoinSessionFailed.Broadcast();

		return;
	}

	if (ConnectString.IsEmpty() || ConnectString.Contains(TEXT(":0")))
	{
		OnJoinSessionFailed.Broadcast();

		return;
	}

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
	}
}

void UCSAdvancedGameInstance::SetMatchType(EMatchType NewType)
{
	MatchType = NewType;
}

void UCSAdvancedGameInstance::ResetLobbySettings()
{
	ExpectedPlayerCount = 0;
	PlayerLobbyDataMap.Empty();
}

void UCSAdvancedGameInstance::HostServerTravel()
{
	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TEXT("/Game/Blueprints/Hud/Maps/LobbyLevel?listen"));
	}
}

void UCSAdvancedGameInstance::StartArcadeMode()
{
	SetMatchType(EMatchType::EMT_Single);
	UGameplayStatics::OpenLevel(this, FName("/Game/Blueprints/Hud/Maps/SingleModeLevel"));
}

void UCSAdvancedGameInstance::ServerTravelToLevel(FName LevelID)
{
	if (!LevelData) return;
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, TEXT("")))
	{
		GetWorld()->ServerTravel(Row->MapPath + TEXT("?listen"));
	}
}

void UCSAdvancedGameInstance::OpenLevelByID(FName LevelID)
{
	if (!LevelData) return;
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, TEXT("")))
	{
		UGameplayStatics::OpenLevel(this, FName(*Row->MapPath));
	}
}

void UCSAdvancedGameInstance::SetPlayerLobbyData(const FString& PlayerName, const FPlayerLobbyData& Data)
{
	PlayerLobbyDataMap.Add(PlayerName, Data);
}

FPlayerLobbyData UCSAdvancedGameInstance::GetPlayerLobbyData(const FString& PlayerName) const
{
	if (const FPlayerLobbyData* Found = PlayerLobbyDataMap.Find(PlayerName))
	{
		return *Found;
	}
	return FPlayerLobbyData();
}

const FCharacterRow* UCSAdvancedGameInstance::FindCharacterRowByJob(EJobTypes Job) const
{
	if (!CharacterData) return nullptr;
	FString EnumString = StaticEnum<EJobTypes>()->GetNameStringByValue((int64)Job);
	EnumString.RemoveFromStart("EJT_");
	return CharacterData->FindRow<FCharacterRow>(FName(*EnumString), TEXT(""));
}

const FLevelRow* UCSAdvancedGameInstance::FindLevelRow(FName RowName) const
{
	if (!LevelData) return nullptr;
	return LevelData->FindRow<FLevelRow>(RowName, TEXT("FindLevelRow"));
}

