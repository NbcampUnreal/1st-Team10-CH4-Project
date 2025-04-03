#include "GameModes/CSLobbyGameMode.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSLobbyGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Controller/CSPlayerController.h"
#include "Managers/CSSpawnManager.h"
#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

ACSLobbyGameMode::ACSLobbyGameMode()
{
	GameStateClass = ACSLobbyGameState::StaticClass();
	PlayerStateClass = ACSPlayerState::StaticClass();
	MatchType = EMatchType::EMT_None;
}

void ACSLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UCSGameInstance* CSGameInstance = GetGameInstance<UCSGameInstance>())
	{
		MatchType = CSGameInstance->MatchType;
	}
}

void ACSLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	SetViewLobbyCam(NewPlayer);

	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(NewPlayer))
	{
		// 로비 전용 UI RPC 필요
		//CSPlayerController->Client_ShowLobbyUI();
	}

	if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(NewPlayer->PlayerState))
	{
		int32 Num = GameState.Get()->PlayerArray.Num();

		CSPlayerState->PlayerIndex = Num;
		CSPlayerState->TeamID = (Num % 2 == 0) ? 1 : 0;
	}

	SetPlayerSelection(NewPlayer, FName("Fingter"));

	PositionLobbyCharacters();
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
	if (const UCSGameInstance* CSGameInstance = GetGameInstance<UCSGameInstance>())
	{
		if (CSGameInstance->SelectedMap != NAME_None)
		{
			// 맵 경로 문자열로 변환
			FString MapPath = CSGameInstance->SelectedMap.ToString();

			// listen 옵션을 붙여서 리슨서버 유지
			FString TravelURL = MapPath + TEXT("?listen");

			// 서버 전용 레벨 이동 함수
			bUseSeamlessTravel = true;
			GetWorld()->ServerTravel(TravelURL);
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
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSSpawnManager::StaticClass(), FoundActors);

	TArray<ACSSpawnManager*> SpawnManagers;
	for (AActor* Actor : FoundActors)
	{
		if (ACSSpawnManager* CSSpawnManager = Cast<ACSSpawnManager>(Actor))
		{
			SpawnManagers.Add(CSSpawnManager);
		}
	}

	// 팀별 슬롯 우선순위 정의 (각 팀마다 가운데가 우선)
	TMap<int32, TArray<ESpawnSlotType>> TeamSlotPriority;
	TeamSlotPriority.Add(0, { ESpawnSlotType::Versus_Team0_Slot0, ESpawnSlotType::Versus_Team0_Slot1, ESpawnSlotType::Versus_Team0_Slot2 });
	TeamSlotPriority.Add(1, { ESpawnSlotType::Versus_Team1_Slot0, ESpawnSlotType::Versus_Team1_Slot1, ESpawnSlotType::Versus_Team1_Slot2 });

	// 팀별 플레이어 목록 구성
	TMap<int32, TArray<ACSPlayerState*>> TeamMap;
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			TeamMap.FindOrAdd(CSPlayerState->TeamID).Add(CSPlayerState);
		}
	}

	// 각 팀에 대해 순서대로 슬롯 배정
	for (auto& TeamEntry : TeamMap)
	{
		int32 TeamID = TeamEntry.Key;
		TArray<ACSPlayerState*>& Players = TeamEntry.Value;
		int32 MaxSlotCount = FMath::Min(Players.Num(), TeamSlotPriority[TeamID].Num());

		for (int32 i = 0; i < MaxSlotCount; ++i)
		{
			ESpawnSlotType SlotType = TeamSlotPriority[TeamID][i];

			for (ACSSpawnManager* SpawnManager : SpawnManagers)
			{
				if (SpawnManager->SlotType == SlotType)
				{
					if (AController* PlayerController = Players[i]->GetOwner<AController>())
					{
						if (APawn* Pawn = PlayerController->GetPawn())
						{
							Pawn->SetActorLocation(SpawnManager->GetActorLocation());
							Pawn->SetActorRotation(SpawnManager->GetActorRotation());
						}
					}
					break;
				}
			}
		}
	}
}

void ACSLobbyGameMode::SetPlayerSelection(APlayerController* Player, FName CharacterID)
{
	if (ACSPlayerState* CSPlayerState = Player->GetPlayerState<ACSPlayerState>())
	{
		CSPlayerState->SelectedCharacterID = CharacterID;
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

void ACSLobbyGameMode::SetViewLobbyCam(APlayerController* NewPlayer)
{
	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("LobbyCamera"), FoundCameras);

	if (FoundCameras.Num() > 0)
	{
		if (ACameraActor* LobbyCam = Cast<ACameraActor>(FoundCameras[0]))
		{
			NewPlayer->SetViewTarget(LobbyCam);
		}
	}
}