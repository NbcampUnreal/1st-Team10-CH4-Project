#include "GameModes/CSLobbyGameMode.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "GameStates/CSLobbyGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Controller/CSPlayerController.h"
#include "Characters/Type/CSLobbyCharacter.h"
#include "Managers/CSSpawnManager.h"
#include "Camera/CameraActor.h"
#include "Data/CSLevelRow.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h" 

ACSLobbyGameMode::ACSLobbyGameMode()
{
	GameStateClass = ACSLobbyGameState::StaticClass();
	PlayerStateClass = ACSPlayerState::StaticClass();
	DefaultPawnClass = nullptr;

	MatchType = EMatchType::EMT_None;
}

void ACSLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UCSAdvancedGameInstance* CSGameInstance = GetGameInstance<UCSAdvancedGameInstance>())
	{
		MatchType = CSGameInstance->GetMatchType();
	}
}

void ACSLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	RegisterPlayerToSession(NewPlayer);

	SetInitPlayerInfo(NewPlayer);

	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(NewPlayer))
	{
		CSPlayerController->Client_ShowLobbyUI();
	}

	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, NewPlayer]()
		{
			SetViewLobbyCam(NewPlayer);
			PositionLobbyCharacters();
			SetSelectedPlayerJob(NewPlayer, EJobTypes::EJT_Fighter);

		}, 1.0f, false);
}

void ACSLobbyGameMode::ReturnAllPlayersToMainMenu()
{
	UWorld* World = GetWorld();
	UCSAdvancedGameInstance* CSGameInstance = GetGameInstance<UCSAdvancedGameInstance>(); 
	if (!World || !CSGameInstance) return;

	if (HasAuthority()) 
	{
		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		if (Subsystem)
		{
			IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
			if (SessionInterface.IsValid())
			{
				SessionInterface->DestroySession(NAME_GameSession);
			}
		}
	}

	const FLevelRow* LevelRow = CSGameInstance->FindLevelRow(FName("MainMenuLevel"));
	if (!LevelRow || LevelRow->MapPath.IsEmpty()) return;

	const FString TravelURL = LevelRow->MapPath; 

	World->ServerTravel(TravelURL);
}

void ACSLobbyGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	if (MatchType == EMatchType::EMT_None)
	{
		if (const UCSAdvancedGameInstance* CSGameInstance = GetGameInstance<UCSAdvancedGameInstance>())
		{
			MatchType = CSGameInstance->GetMatchType();
		}
	}

	ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(C);
	if (!CSPlayerController) return;

	CSPlayerController->Client_ShowLobbyUI();

	if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(C->PlayerState))
	{
		int32 Num = GameState.Get()->PlayerArray.Num();

		CSPlayerState->PlayerIndex = Num;

		if (MatchType == EMatchType::EMT_Versus)
		{
			CSPlayerState->TeamID = (Num % 2 == 0) ? 1 : 0;
		}
		else if (MatchType == EMatchType::EMT_Coop)
		{
			CSPlayerState->TeamID = 0;
		}
	}

	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, CSPlayerController]()
		{
			SetViewLobbyCam(CSPlayerController);
			PositionLobbyCharacters();
			SetSelectedPlayerJob(CSPlayerController, EJobTypes::EJT_Fighter);

		}, 1.0f, false);
}

void ACSLobbyGameMode::StartMatchIfReady()
{
	const ACSLobbyGameState* LobbyGameState = GetGameState<ACSLobbyGameState>();
	if (!LobbyGameState) return;

	if (!LobbyGameState->AreAllPlayerReady()) return;

	switch (MatchType)
	{
	case EMatchType::EMT_Versus:
		if (NumPlayers >= 2 && NumPlayers % 2 == 0 && IsTeamBalanced())
		{
			TryStartMatch();
		}
		break;

	case EMatchType::EMT_Coop:
		if (NumPlayers >= 2 && NumPlayers <= 4)
		{
			TryStartMatch();
		}
		break;

	default:
		break;
	}
}

void ACSLobbyGameMode::TryStartMatch()
{
	const ACSLobbyGameState* LobbyGameState = GetGameState<ACSLobbyGameState>();
	if (!LobbyGameState || LobbyGameState->SelectedMap == NAME_None) return;

	if (UCSAdvancedGameInstance* CSGameInstance = GetGameInstance<UCSAdvancedGameInstance>())
	{
		const int32 PlayerCount = GameState->PlayerArray.Num();
		CSGameInstance->ExpectedPlayerCount = PlayerCount;

		if (CSGameInstance->LevelData)
		{
			const FString ContextStr = TEXT("TryStartMatch");
			const FLevelRow* LevelRow = CSGameInstance->LevelData->FindRow<FLevelRow>(LobbyGameState->SelectedMap, ContextStr);
			if (LevelRow && !LevelRow->MapPath.IsEmpty())
			{
				const FString TravelURL = LevelRow->MapPath + TEXT("?listen");

				bUseSeamlessTravel = true;
				GetWorld()->ServerTravel(TravelURL);
			}
		}
	}
}

void ACSLobbyGameMode::ChangeTeam(APlayerController* Player)
{
	if (ACSPlayerState* CSPlayerState = Player->GetPlayerState<ACSPlayerState>())
	{
		int32 CurrentTeam = CSPlayerState->TeamID;
		int32 NewTeam = (CurrentTeam == 0) ? 1 : 0;

		CSPlayerState->TeamID = NewTeam;

		PositionLobbyCharacters();
	}
}

void ACSLobbyGameMode::PositionLobbyCharacters()
{
	switch (MatchType)
	{
	case EMatchType::EMT_Versus:
		PositionVersusCharacters();
		break;

	case EMatchType::EMT_Coop:
		PositionCoopCharacters();
		break;

	default:
		break;
	}
}

void ACSLobbyGameMode::PositionVersusCharacters()
{
	const auto TeamSlotPriority = GetSlotPriorityForMatchType();

	TMap<int32, TArray<ACSPlayerState*>> TeamMap;
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			TeamMap.FindOrAdd(CSPlayerState->TeamID).Add(CSPlayerState);
		}
	}

	TArray<ACSSpawnManager*> SpawnManagers;
	GetAllSpawnManagers(SpawnManagers);

	for (auto& Entry : TeamMap)
	{
		int32 TeamID = Entry.Key;
		TArray<ACSPlayerState*>& Players = Entry.Value;

		for (int32 i = 0; i < Players.Num(); ++i)
		{
			if (TeamSlotPriority.Contains(TeamID) && i < TeamSlotPriority[TeamID].Num())
			{
				AssignPlayerToSlot(Players[i], TeamSlotPriority[TeamID][i], SpawnManagers);
			}
		}
	}
}

void ACSLobbyGameMode::PositionCoopCharacters()
{
	TMap<int32, TArray<ESpawnSlotType>> SlotMap = GetSlotPriorityForMatchType();
	if (!SlotMap.Contains(0)) return;
	const TArray<ESpawnSlotType>& SlotList = SlotMap[0];

	TArray<ACSSpawnManager*> SpawnManagers;
	GetAllSpawnManagers(SpawnManagers);

	int32 Index = 0;
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			if (Index >= SlotList.Num()) break;
			AssignPlayerToSlot(CSPlayerState, SlotList[Index], SpawnManagers);
			++Index;
		}
	}
}

TMap<int32, TArray<ESpawnSlotType>> ACSLobbyGameMode::GetSlotPriorityForMatchType() const
{
	TMap<int32, TArray<ESpawnSlotType>> Result;

	switch (MatchType)
	{
	case EMatchType::EMT_Versus:
		Result.Add(0, { ESpawnSlotType::Versus_Team0_Slot0, ESpawnSlotType::Versus_Team0_Slot1, ESpawnSlotType::Versus_Team0_Slot2 });
		Result.Add(1, { ESpawnSlotType::Versus_Team1_Slot0, ESpawnSlotType::Versus_Team1_Slot1, ESpawnSlotType::Versus_Team1_Slot2 });
		break;

	case EMatchType::EMT_Coop:
		Result.Add(0, {
			ESpawnSlotType::Coop_Player_Slot0,
			ESpawnSlotType::Coop_Player_Slot1,
			ESpawnSlotType::Coop_Player_Slot2,
			ESpawnSlotType::Coop_Player_Slot3 });
		break;

	default:
		break;
	}

	return Result;
}

void ACSLobbyGameMode::GetAllSpawnManagers(TArray<ACSSpawnManager*>& OutManagers)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSSpawnManager::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (ACSSpawnManager* SpawnManager = Cast<ACSSpawnManager>(Actor))
		{
			OutManagers.Add(SpawnManager);
		}
	}
}

void ACSLobbyGameMode::AssignPlayerToSlot(ACSPlayerState* PlayerState, ESpawnSlotType SlotType, const TArray<ACSSpawnManager*>& Managers)
{
	for (ACSSpawnManager* Manager : Managers)
	{
		if (Manager->SlotType == SlotType)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(PlayerState->GetOwner()))
			{
				SpawnOrUpdateLobbyCharacter(PlayerController, Manager->GetActorLocation(), Manager->GetActorRotation());
			}
			break;
		}
	}
}

void ACSLobbyGameMode::SpawnOrUpdateLobbyCharacter(APlayerController* PlayerController, const FVector& Location, const FRotator& Rotation)
{
	if (!PlayerController) return;
	
	ACSLobbyCharacter* LobbyCharacter = nullptr;

	if (LobbyCharacterMap.Contains(PlayerController))
	{
		LobbyCharacter = LobbyCharacterMap[PlayerController];
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PlayerController;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		if (!LobbyCharacterClass) return;
		
		LobbyCharacter = GetWorld()->SpawnActor<ACSLobbyCharacter>(LobbyCharacterClass, Location, Rotation, SpawnParams);
		if (LobbyCharacter)
		{
			LobbyCharacterMap.Add(PlayerController, LobbyCharacter);
		}
	}

	if (LobbyCharacter)
	{
		LobbyCharacter->SetActorLocation(Location);
		LobbyCharacter->SetActorRotation(Rotation);
	}
}

void ACSLobbyGameMode::SetSelectedPlayerJob(APlayerController* Player, EJobTypes NewJob)
{
	if (ACSPlayerState* CSPlayerState = Player->GetPlayerState<ACSPlayerState>())
	{
		CSPlayerState->SelectedJob = NewJob;

		FPlayerLobbyData LobbyData;
		LobbyData.SelectedJob = CSPlayerState->SelectedJob;
		LobbyData.PlayerIndex = CSPlayerState->PlayerIndex;
		LobbyData.TeamID = CSPlayerState->TeamID;

		if (UCSAdvancedGameInstance* CSGameInstance = GetGameInstance<UCSAdvancedGameInstance>())
		{
			CSGameInstance->SetPlayerLobbyData(CSPlayerState->GetPlayerName(), LobbyData);
		}

		if (ACSLobbyCharacter* LobbyCharacter = LobbyCharacterMap.FindRef(Player))
		{
			LobbyCharacter->Multicast_UpdateMesh(NewJob);
		}
	}
}

bool ACSLobbyGameMode::IsTeamBalanced()
{
	int32 Team_0 = 0;
	int32 Team_1 = 0;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			(CSPlayerState->TeamID == 0) ? Team_0++ : Team_1++;
		}
	}

	return Team_0 == Team_1;
}

void ACSLobbyGameMode::RegisterPlayerToSession(APlayerController* NewPlayer)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			APlayerState* PlayerState = NewPlayer->PlayerState;
			if (PlayerState)
			{
				FUniqueNetIdRepl ReplId = PlayerState->GetUniqueId();
				if (ReplId.IsValid())
				{
					const TSharedPtr<const FUniqueNetId> NetId = ReplId.GetUniqueNetId();
					if (NetId.IsValid())
					{
						const FName SessionName = NAME_GameSession;
						const bool bWasSuccessful = SessionInterface->RegisterPlayer(SessionName, *NetId, false);
					}
				}
			}
		}
	}
}

void ACSLobbyGameMode::SetInitPlayerInfo(APlayerController* NewPlayer)
{
	if (MatchType == EMatchType::EMT_None)
	{
		if (const UCSAdvancedGameInstance* CSGameInstance = GetGameInstance<UCSAdvancedGameInstance>())
		{
			MatchType = CSGameInstance->GetMatchType();
		}
	}

	if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(NewPlayer->PlayerState))
	{
		int32 Num = GameState.Get()->PlayerArray.Num();

		CSPlayerState->PlayerIndex = Num;

		if (MatchType == EMatchType::EMT_Versus)
		{
			CSPlayerState->TeamID = (Num % 2 == 0) ? 1 : 0;
		}
		else if (MatchType == EMatchType::EMT_Coop)
		{
			CSPlayerState->TeamID = 0;
		}
	}
}

void ACSLobbyGameMode::SetViewLobbyCam(APlayerController* NewPlayer)
{
	FName CameraTag;
	switch (MatchType)
	{
	case EMatchType::EMT_Versus:
		CameraTag = FName("LobbyCamera_Versus");
		break;

	case EMatchType::EMT_Coop:
		CameraTag = FName("LobbyCamera_Coop");
		break;
	}

	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CameraTag, FoundCameras);

	if (FoundCameras.Num() > 0)
	{
		if (ACameraActor* LobbyCam = Cast<ACameraActor>(FoundCameras[0]))
		{
			NewPlayer->SetViewTarget(LobbyCam);
		}
	}
}