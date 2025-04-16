#include "GameModes/CSVersusGameMode.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "GameStates/CSVersusGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Controller/CSPlayerController.h"
#include "Characters/CSBaseCharacter.h"
#include "Characters/CSPlayerCharacter.h"
#include "Managers/CSSpawnManager.h"
#include "Kismet/GameplayStatics.h"

ACSVersusGameMode::ACSVersusGameMode()
{
	PrimaryActorTick.bCanEverTick = false;

	MatchTimerHandle.Invalidate();
	ReturnToLobbyHandle.Invalidate();

    MatchTimeLimit = 90.0f;
	AlivePlayersPerTeam = { 0,0 };
	SlotCursor = { 0,0 };

    GameStateClass = ACSVersusGameState::StaticClass();
}

void ACSVersusGameMode::BeginPlay()
{
    Super::BeginPlay();

	VersusGameState = GetGameState<ACSVersusGameState>();
}

void ACSVersusGameMode::InitGameLogic()
{
	const int32 AlivePlayers = LoggedInPlayerCount / 2;
	AlivePlayersPerTeam = { AlivePlayers,AlivePlayers };

	if (VersusGameState)
	{
		VersusGameState->bIsSuddenDeath = false;
	}
	
	SpawnAllPlayers();

	HandleStartGame();
}

void ACSVersusGameMode::UpdateMatchTimer()
{
	Super::UpdateMatchTimer();
	
	if (BaseGameState && BaseGameState->GetRemainingMatchTime() <= 0)
	{
		TriggerSuddenDeath();
	}
}

void ACSVersusGameMode::HandlePlayerDeath(AController* DeadPlayer)
{
	if (ACSPlayerState* CSPlayerState = DeadPlayer ? DeadPlayer->GetPlayerState<ACSPlayerState>() : nullptr)
	{
		CSPlayerState->bIsAlive = false;

		UpdateAliveTeams(CSPlayerState);

		CheckWinCondition();
	}
}

void ACSVersusGameMode::UpdateAliveTeams(ACSPlayerState* PlayerState)
{
	if (PlayerState)
	{
		(PlayerState->TeamID == 0) ? AlivePlayersPerTeam.X-- : AlivePlayersPerTeam.Y--;
	}
}

void ACSVersusGameMode::CheckWinCondition()
{
	if (AlivePlayersPerTeam.X == 0 && AlivePlayersPerTeam.Y > 0)
	{
		FinishMatch(1);
	}
	else if (AlivePlayersPerTeam.Y == 0 && AlivePlayersPerTeam.X > 0)
	{
		FinishMatch(0);
	}
}

void ACSVersusGameMode::TriggerSuddenDeath()
{
	if (!VersusGameState) return;

	VersusGameState->bIsSuddenDeath = true;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			if (!CSPlayerState->bIsAlive) continue;

			if (APlayerController* PlayerController = Cast<APlayerController>(CSPlayerState->GetOwner()))
			{
				if (APawn* Pawn = PlayerController->GetPawn())
				{
					if (ACSPlayerCharacter* Character = Cast<ACSPlayerCharacter>(Pawn))
					{
						Character->ActivateSuddenDeathMode();
					}
				}
			}
		}
	}
}

void ACSVersusGameMode::FinishMatch(int32 WinningTeamID)
{
	if (VersusGameState)
	{
		VersusGameState->WinningTeamID = WinningTeamID;
		VersusGameState->WinningPlayers.Empty();

		for (APlayerState* PlayerState : VersusGameState->PlayerArray)
		{
			if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
			{
				if (CSPlayerState->TeamID == WinningTeamID)
				{
					VersusGameState->WinningPlayers.Add(CSPlayerState);
				}
			}
		}
	}

	HandleEndGame();

	// 승리 연출 구현 필요(카메라 전환, 승리 팀 캐릭터 배치 등)
	// UI는 OnMatchPhaseChanged(EMatchPhase MatchPhase) 함수에서 적용

	GetWorld()->GetTimerManager().SetTimer(ReturnToLobbyHandle, this, &ACSVersusGameMode::ReturnToLobby, 10.0f, false);
}

ESpawnSlotType ACSVersusGameMode::GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState)
{
	if (!PlayerState) return ESpawnSlotType::None;

	int32 TeamID = PlayerState->TeamID;
	int32 Index = (TeamID == 0) ? SlotCursor.X++ : SlotCursor.Y++;

	const int32 Base = (TeamID == 0)
		? static_cast<int32>(ESpawnSlotType::Versus_Team0_Slot0)
		: static_cast<int32>(ESpawnSlotType::Versus_Team1_Slot0);

	ESpawnSlotType Result = static_cast<ESpawnSlotType>(Base + Index);

	return Result;
}
