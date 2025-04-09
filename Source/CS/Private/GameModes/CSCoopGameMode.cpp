#include "GameModes/CSCoopGameMode.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSCoopGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Managers/CSSpawnManager.h"
#include "AI/Controller/AIBaseController.h"
#include "Data/CSAIRow.h"
#include "Kismet/GameplayStatics.h"

ACSCoopGameMode::ACSCoopGameMode()
{
	PrimaryActorTick.bCanEverTick = false;

	MatchTimeLimit = 90;
	AlivePlayerCount = -1;
	RemainingEnemyCount = -1;

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

void ACSCoopGameMode::HandleStartGame()
{
	Super::HandleStartGame();

	AllAIStartLogic(PendingAIPawns);
	PendingAIPawns.Empty();
}

void ACSCoopGameMode::SpawnAIEnemies()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSSpawnManager::StaticClass(), FoundActors);

	if (FoundActors.IsEmpty()) return;

	TMap<ESpawnSlotType, ACSSpawnManager*> SlotMap = FindAllSpawnManager();
	if (SlotMap.IsEmpty()) return;

	for (int32 i = 0; i < RemainingEnemyCount; ++i)
	{
		ESpawnSlotType AISlot = GetSpawnSlotForAI(i);

		if (ACSSpawnManager* SpawnPoint = SlotMap.FindRef(AISlot))
		{
			FVector SpawnLocation = SpawnPoint->GetActorLocation();
			FRotator SpawnRotation = SpawnPoint->GetActorRotation();

			TSubclassOf<APawn> RandomAIClass = SelectRandomAIClass();

			if (RandomAIClass)
			{
				APawn* SpawnedAI = GetWorld()->SpawnActor<APawn>(RandomAIClass, SpawnLocation, SpawnRotation);
				if (SpawnedAI)
				{
					PendingAIPawns.Add(SpawnedAI);
				}
			}
		}
	}
}

TSubclassOf<APawn> ACSCoopGameMode::SelectRandomAIClass()
{
	if (!CSGameInstance || !CSGameInstance->AIData) return nullptr;

	const TArray<FName> RowNames = CSGameInstance->AIData->GetRowNames();
	if (RowNames.IsEmpty()) return nullptr;

	const FName SelectedRow = RowNames[FMath::RandRange(0, RowNames.Num() - 1)];
	const FAIRow* Row = CSGameInstance->AIData->FindRow<FAIRow>(SelectedRow, TEXT("SelectRandomAIClass"));

	if (!Row || !Row->AICharacterClass.IsValid()) return nullptr;

	return Row->AICharacterClass.LoadSynchronous();
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

	if (BaseGameState && BaseGameState->GetRemainingMatchTime() <= 0)
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
		bPlayersWin ? CoopGameState->SetMatchResult(EMatchResult::EMR_Win) : CoopGameState->SetMatchResult(EMatchResult::EMR_Lose);
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
