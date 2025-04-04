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
    PrimaryActorTick.bCanEverTick = true;

	MatchTimerHandle.Invalidate();
	ReturnToLobbyHandle.Invalidate();

    MatchTimeLimit = 90.0f;

	AlivePlayersPerTeam = { 0,0 };
	LoggedInPlayerCount = 0;
	ExpectedPlayerCount = 0;

    GameStateClass = ACSVersusGameState::StaticClass();
}

void ACSVersusGameMode::BeginPlay()
{
    Super::BeginPlay();

	CSGameInstance = GetGameInstance<UCSGameInstance>();
	CSGameState = GetGameState<ACSVersusGameState>();

	if (CSGameInstance)
	{
		ExpectedPlayerCount = CSGameInstance->ExpectedPlayerCount;
	}
}

void ACSVersusGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	LoggedInPlayerCount++;

	if (LoggedInPlayerCount == ExpectedPlayerCount)
	{
		InitVersusLogic();
	}
}

void ACSVersusGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}

void ACSVersusGameMode::InitVersusLogic()
{
	int32 AlivePlayers = LoggedInPlayerCount / 2;
	AlivePlayersPerTeam = { AlivePlayers,AlivePlayers };

	CSGameState->bIsSuddenDeath = false;

    SpawnPlayerAtTeamSlots();
	HandleStartGame();
}

void ACSVersusGameMode::HandleStartGame()
{
	Super::HandleStartGame();

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState);
		if (!CSPlayerState) return;

		APlayerController* PlayerController = Cast<APlayerController>(CSPlayerState->GetOwner());
		if (!PlayerController) return;

		APawn* Pawn = PlayerController->GetPawn();
		if (!Pawn) return;

		Pawn->EnableInput(PlayerController);
	}

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

	TMap<int32, int32> TeamSlotIndex;
	TeamSlotIndex.Add(0, 0); // Team 0 -> Slot0
	TeamSlotIndex.Add(1, 0); // Team 1 -> Slot0

	if (!CSGameInstance || !CSGameInstance->CharacterData) return;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			int32 TeamID = CSPlayerState->TeamID;
			int32 SlotIndex = TeamSlotIndex[TeamID];

			ESpawnSlotType SlotType = (TeamID == 0)
				? (SlotIndex == 0 ? ESpawnSlotType::Versus_Team0_Slot0 : ESpawnSlotType::Versus_Team0_Slot1)
				: (SlotIndex == 0 ? ESpawnSlotType::Versus_Team1_Slot0 : ESpawnSlotType::Versus_Team1_Slot1);

			if (ACSSpawnManager* SpawnPoint = SlotMap.FindRef(SlotType))
			{
				if (APlayerController* PlayerController = Cast<APlayerController>(CSPlayerState->GetOwner()))
				{
					FString Context = TEXT("Spawning Character");
					const FCharacterRow* Row = CSGameInstance->CharacterData->FindRow<FCharacterRow>(CSPlayerState->SelectedCharacterID, Context);

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
	if (CSGameState)
	{
		CSGameState->RemainingMatchTime = MatchTimeLimit;
	}

	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ACSVersusGameMode::UpdateMatchTime, 1.0f, true);
}

void ACSVersusGameMode::UpdateMatchTime()
{
	if (CSGameState)
	{
		CSGameState->RemainingMatchTime--;

		if (CSGameState->RemainingMatchTime <= 0)
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
		// 1. 죽은 표시
		CSPlayerState->bIsAlive = false;

		// 2. 팀 생존자 수 갱신
		UpdateAliveTeams(CSPlayerState);

		// 3. 승리 조건 판별
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
	CSGameState->bIsSuddenDeath = true;

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
	HandleEndGame();

	// 승리 연출 구현 필요(카메라 전환, 승리 팀 캐릭터 배치 등)
	// UI는 OnMatchPhaseChanged(EMatchPhase MatchPhase) 함수에서 적용

	GetWorld()->GetTimerManager().SetTimer(ReturnToLobbyHandle, this, &ACSVersusGameMode::ReturnToLobby, 10.0f, false);
}

void ACSVersusGameMode::ReturnToLobby()
{
	if (CSGameInstance)
	{
		CSGameInstance->ResetLobbySettings();
	}

	for (APlayerState* PlayerState : CSGameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			CSPlayerState->ResetLobbySettings(); 
		}
	}

	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel(TEXT("/Game/Maps/Lobby?listen")); // 맵 정리되면 수정 필요
}

