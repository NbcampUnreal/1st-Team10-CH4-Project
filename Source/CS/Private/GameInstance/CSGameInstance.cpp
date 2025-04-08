#include "GameInstance/CSGameInstance.h"
#include "Controller/CSPlayerController.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
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
		UE_LOG(LogTemp, Log, TEXT("Session %s 생성 성공"), *SessionName.ToString());
		GetWorld()->ServerTravel("/Game/Maps/LobbyLevel?listen");
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create session: %s"), *SessionName.ToString());
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

void UCSGameInstance::StartingSinglePlay()
{
	if (!GetWorld()) return;

	UGameplayStatics::OpenLevel(GetWorld(), FName("Single_Level"));
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

void UCSGameInstance::ResetLobbySettings()
{
	ExpectedPlayerCount = 0;
}


