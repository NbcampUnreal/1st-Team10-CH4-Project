#include "GameInstance/CSGameInstance.h"
#include "Controller/CSPlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Data/CSLevelRow.h"
#include "Kismet/GameplayStatics.h"

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
		}
	}
}

void UCSGameInstance::HostSession(FName SessionName)
{
	if(!SessionInterface.IsValid()) return;

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = 4;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = false;

	SessionInterface->CreateSession(0, SessionName, SessionSettings);
}

void UCSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		ServerTravelToLevel(FName("LobbyLevel"));
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 세션 생성 실패: %s"), *SessionName.ToString());
	}
}

void UCSGameInstance::FindSessions()
{
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10;

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UCSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Found %d sessions"), SessionSearch->SearchResults.Num());

		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults)
		{
			if (Result.IsValid())
			{
				JoinFoundSession(Result);
				return;
			}
		}
	}

	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
	{
		/*CSPlayerController->Client_ShowNoSessionPopup();*/
	}
}

void UCSGameInstance::JoinFoundSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!SessionInterface.IsValid()) return;
	SessionInterface->JoinSession(0, NAME_GameSession, SearchResult);
}

void UCSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	FString ConnectString;
	if (SessionInterface->GetResolvedConnectString(SessionName, ConnectString))
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] 세션 참가 불가 %s"), *SessionName.ToString());
	}
}

void UCSGameInstance::StartingSinglePlay()
{
	if (!GetWorld()) return;

	OpenLevelByID(FName("SingleModeLevel"));
}

void UCSGameInstance::ServerTravelToLevel(FName LevelID)
{
	if (!LevelData) return;

	const FString ContextStr = TEXT("ServerTravelToLevel");
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, ContextStr))
	{
		if (!Row->MapPath.IsEmpty())
		{
			FString TravelURL = Row->MapPath + TEXT("?listen");
			GetWorld()->ServerTravel(TravelURL);
		}
	}
}

void UCSGameInstance::OpenLevelByID(FName LevelID)
{
	if (!LevelData) return;

	const FString ContextStr = TEXT("OpenLevelByID");
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, ContextStr))
	{
		if (!Row->MapPath.IsEmpty())
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName(*Row->MapPath));
		}
	}
}

void UCSGameInstance::ResetLobbySettings()
{
	ExpectedPlayerCount = 0;
}

const FCharacterRow* UCSGameInstance::FindCharacterRowByJob(EJobTypes Job) const
{
	if (!CharacterData) return nullptr;

	const UEnum* EnumPtr = StaticEnum<EJobTypes>();
	if (!EnumPtr) return nullptr;

	FString RowNameString = EnumPtr->GetNameStringByValue((int64)Job);
	FName RowName(*RowNameString);

	return CharacterData->FindRow<FCharacterRow>(RowName, TEXT("FindCharacterRowByJob"));
}
