#include "GameModes/CSCoopGameMode.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSCoopGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Managers/CSSpawnManager.h"
#include "Kismet/GameplayStatics.h"

ACSCoopGameMode::ACSCoopGameMode()
{
	PrimaryActorTick.bCanEverTick = false;

	MatchTimeLimit = 90;
	AlivePlayerCount = -1;
	RemainingEnemyCount = -1;

	EnemyAIPawnClass = nullptr;

	GameStateClass = ACSCoopGameState::StaticClass();
}

void ACSCoopGameMode::BeginPlay()
{
	Super::BeginPlay();

	CoopGameState = GetGameState<ACSCoopGameState>();
}

void ACSCoopGameMode::InitGameLogic()
{
	AlivePlayerCount = LoggedInPlayerCount;
	RemainingEnemyCount = LoggedInPlayerCount;

	SpawnAllPlayers();
	SpawnAIEnemies();

	HandleStartGame();
}

void ACSCoopGameMode::SpawnAIEnemies()
{
	if (!EnemyAIPawnClass) return;

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

	for (int32 i = 0; i < RemainingEnemyCount; ++i)
	{
		ESpawnSlotType AISlot = GetSpawnSlotForAI(i);

		if (ACSSpawnManager* SpawnPoint = SlotMap.FindRef(AISlot))
		{
			FVector SpawnLocation = SpawnPoint->GetActorLocation();
			FRotator SpawnRotation = SpawnPoint->GetActorRotation();

			APawn* SpawnedAI = GetWorld()->SpawnActor<APawn>(EnemyAIPawnClass, SpawnLocation, SpawnRotation);
		}
	}
}

void ACSCoopGameMode::HandlePlayerDeath(AController* DeadPlayer)
{
	if (ACSPlayerState* CSPlayerState = DeadPlayer ? DeadPlayer->GetPlayerState<ACSPlayerState>() : nullptr)
	{
		CSPlayerState->bIsAlive = false;

		AlivePlayerCount--;

		if (AlivePlayerCount <= 0)
		{
			FinishMatch(false);
		}
	}
}

void ACSCoopGameMode::HandleAIDeath(AActor* DeadAI)
{
	RemainingEnemyCount--;

	CheckWinCondition();
}

void ACSCoopGameMode::UpdateMatchTimer()
{
	Super::UpdateMatchTimer();

	if (BaseGameState && BaseGameState->RemainingMatchTime <= 0)
	{
		FinishMatch(false); 
	}
}

void ACSCoopGameMode::CheckWinCondition()
{
	if (RemainingEnemyCount <= 0)
	{
		FinishMatch(true);
	}
}

void ACSCoopGameMode::FinishMatch(bool bPlayersWin)
{
	if (CoopGameState)
	{
		CoopGameState->MatchResult = bPlayersWin ? EMatchResult::EMR_Win : EMatchResult::EMR_Lose;
	}

	HandleEndGame();

	GetWorldTimerManager().SetTimer(ReturnToLobbyHandle, this, &ACSCoopGameMode::ReturnToLobby, 10.0f, false);
}

ESpawnSlotType ACSCoopGameMode::GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState) const
{
	if (!PlayerState) return ESpawnSlotType::None;

	int32 Index = PlayerState->PlayerIndex;
	return static_cast<ESpawnSlotType>(static_cast<int32>(ESpawnSlotType::Coop_Player_Slot0) + Index - 1);
}

ESpawnSlotType ACSCoopGameMode::GetSpawnSlotForAI(int32 Index) const
{
	return static_cast<ESpawnSlotType>(
		static_cast<int32>(ESpawnSlotType::Coop_AI_Slot0) + Index
		);
}
