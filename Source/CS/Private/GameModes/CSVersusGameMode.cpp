#include "GameModes/CSVersusGameMode.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSVersusGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Controller/CSPlayerController.h"
#include "Characters/CSBaseCharacter.h"
#include "Managers/CSSpawnManager.h"
#include "Kismet/GameplayStatics.h"

ACSVersusGameMode::ACSVersusGameMode()
{
	PrimaryActorTick.bCanEverTick = false;

	MatchTimerHandle.Invalidate();
	ReturnToLobbyHandle.Invalidate();

    MatchTimeLimit = 90.0f;
	AlivePlayersPerTeam = { 0,0 };

    GameStateClass = ACSVersusGameState::StaticClass();
}

void ACSVersusGameMode::BeginPlay()
{
    Super::BeginPlay();

	VersusGameState = GetGameState<ACSVersusGameState>();
}

void ACSVersusGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
}

void ACSVersusGameMode::InitGameLogic()
{
	const int32 AlivePlayers = LoggedInPlayerCount / 2;
	AlivePlayersPerTeam = { AlivePlayers,AlivePlayers };

	if (VersusGameState)
	{
		VersusGameState->bIsSuddenDeath = false;
	}
	
    SpawnPlayerAtTeamSlots();
	HandleStartGame();
}

void ACSVersusGameMode::HandleStartGame()
{
	Super::HandleStartGame();

	StartMatchTimeCountDown();
}

void ACSVersusGameMode::SpawnPlayerAtTeamSlots()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSSpawnManager::StaticClass(), FoundActors);

	TMap<ESpawnSlotType, ACSSpawnManager*> SlotMap;
	for (AActor* Actor : FoundActors)
	{
		if (ACSSpawnManager* SpawnManager = Cast<ACSSpawnManager>(Actor))
		{
			SlotMap.Add(SpawnManager->SlotType, SpawnManager);
		}
	}

	TMap<int32, int32> TeamSlotIndex{ {0, 0}, {1, 0} };

	if (!CSGameInstance || !CSGameInstance->CharacterData) return;

	for (APlayerState* PlayerState : VersusGameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			const int32 TeamID = CSPlayerState->TeamID;
			const int32 SlotIndex = TeamSlotIndex[TeamID];

			const ESpawnSlotType SlotType = (TeamID == 0)
				? (SlotIndex == 0 ? ESpawnSlotType::Versus_Team0_Slot0 : ESpawnSlotType::Versus_Team0_Slot1)
				: (SlotIndex == 0 ? ESpawnSlotType::Versus_Team1_Slot0 : ESpawnSlotType::Versus_Team1_Slot1);

			if (ACSSpawnManager* SpawnPoint = SlotMap.FindRef(SlotType))
			{
				if (APlayerController* PlayerController = Cast<APlayerController>(CSPlayerState->GetOwner()))
				{
					const FCharacterRow* Row = CSGameInstance->CharacterData->FindRow<FCharacterRow>(CSPlayerState->SelectedCharacterID, TEXT("Spawn"));
					if (!Row || !Row->CharacterClass.IsValid()) continue;

					TSubclassOf<APawn> CharacterClass = Row->CharacterClass.LoadSynchronous();
					
					FVector SpawnLocation = SpawnPoint->GetActorLocation();
					FRotator SpawnRotation = SpawnPoint->GetActorRotation();
					APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(CharacterClass, SpawnLocation, SpawnRotation);

					if (SpawnedPawn)
					{
						PlayerController->Possess(SpawnedPawn);
						SpawnedPawn->DisableInput(PlayerController);
					}
				}
			}

			TeamSlotIndex[TeamID]++;
		}
	}
}

void ACSVersusGameMode::StartMatchTimeCountDown()
{
	if (VersusGameState)
	{
		VersusGameState->RemainingMatchTime = MatchTimeLimit;
	}

	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ACSVersusGameMode::UpdateMatchTime, 1.0f, true);
}

void ACSVersusGameMode::UpdateMatchTime()
{
	if (VersusGameState)
	{
		VersusGameState->RemainingMatchTime--;

		if (VersusGameState->RemainingMatchTime <= 0)
		{
			GetWorldTimerManager().ClearTimer(MatchTimerHandle);

			TriggerSuddenDeath();
		}
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
					if (ACSBaseCharacter* Character = Cast<ACSBaseCharacter>(Pawn))
					{
						/*Character->ActivateSuddenDeathMode();*/ // 캐릭터 공격력 증가하는 함수 필요.
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

void ACSVersusGameMode::ReturnToLobby()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ReturnToLobbyHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ReturnToLobbyHandle);
	}

	if (CSGameInstance)
	{
		CSGameInstance->ResetLobbySettings();
	}

	for (APlayerState* PlayerState : VersusGameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			CSPlayerState->ResetLobbySettings(); 
		}
	}

	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel(TEXT("/Game/Maps/Lobby?listen")); // 맵 정리되면 수정 필요
}

