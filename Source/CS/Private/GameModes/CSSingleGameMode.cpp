#include "GameModes/CSSingleGameMode.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSSingleGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "Data/CSLevelRow.h"
#include "Kismet/GameplayStatics.h"

ACSSingleGameMode::ACSSingleGameMode()
{
	GameStateClass = ACSSingleGameState::StaticClass();

	TotalAICount = 0;
	DeadAICount = 0;

	BossGateActor = nullptr;
	BossCharacterClass = nullptr;
}

void ACSSingleGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (CSGameInstance)
	{
		CSGameInstance->ExpectedPlayerCount = 1;
	}

	SingleGameState = GetGameState<ACSSingleGameState>();
}

void ACSSingleGameMode::InitGameLogic()
{
	CountInitialAI();
	UpdateGameStateAIStatus();

	SpawnAllPlayers();
	HandleStartGame();
}

void ACSSingleGameMode::HandleStartGame()
{
	Super::HandleStartGame();

	AllAIStartLogic(PendingAIPawns);
	PendingAIPawns.Empty();
}

void ACSSingleGameMode::HandlePlayerDeath(AController* DeadPlayer)
{
	if (ACSPlayerState* CSPlayerState = DeadPlayer ? DeadPlayer->GetPlayerState<ACSPlayerState>() : nullptr)
	{
		CSPlayerState->bIsAlive = false;

		if (SingleGameState)
		{
			SingleGameState->SetMatchResult(EMatchResult::EMR_Lose);
		}

		HandleEndGame();
	}
}

void ACSSingleGameMode::HandleAIDeath(AActor* DeadAI)
{
	if (DeadAI->ActorHasTag("Boss"))
	{
		if (SingleGameState)
		{
			SingleGameState->SetMatchResult(EMatchResult::EMR_Win);
		}

		HandleEndGame();
	}

	else
	{
		DeadAICount++;
		UpdateGameStateAIStatus();

		if (DeadAICount >= TotalAICount)
		{
			OpenBossGate();
			StartBossPhase();
		}
	}
}

ESpawnSlotType ACSSingleGameMode::GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState) const
{
	return ESpawnSlotType::Single_Player_Slot0;
}

void ACSSingleGameMode::CountInitialAI()
{
	PendingAIPawns.Empty();
	TotalAICount = 0;
	DeadAICount = 0;

	TArray<AActor*> FoundAI;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAIBaseCharacter::StaticClass(), FoundAI);

	if (FoundAI.IsEmpty()) return;

	for (AActor* AI : FoundAI)
	{
		if (AAIBaseCharacter* AICharacter = Cast<AAIBaseCharacter>(AI))
		{
			if (AICharacter->GetController())
			{
				PendingAIPawns.Add(AICharacter);
				TotalAICount++;
			}
		}
	}
}

void ACSSingleGameMode::OpenBossGate()
{
	if (BossGateActor)
	{
		// 문 열림 연출
	}
}

void ACSSingleGameMode::StartBossPhase()
{
	if (SingleGameState)
	{
		SingleGameState->SetBossPhase(true);
	}

	SpawnBoss();
}

void ACSSingleGameMode::SpawnBoss()
{
	if(!BossCharacterClass) return;

	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BossSpawnPoint"), SpawnPoints);

	if (SpawnPoints.IsEmpty()) return;

	const FVector Location = SpawnPoints[0]->GetActorLocation();
	const FRotator Rotation = SpawnPoints[0]->GetActorRotation();

	APawn* SpawnedBoss = GetWorld()->SpawnActor<APawn>(BossCharacterClass, Location, Rotation);
	if (SpawnedBoss)
	{
		if (SpawnedBoss && SpawnedBoss->GetController())
		{
			if (AAIBaseController* AIController = Cast<AAIBaseController>(SpawnedBoss->GetController()))
			{
				AIController->StartLogicAI();
			}
		}
	}
}

void ACSSingleGameMode::RestartSingleGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()));
}

void ACSSingleGameMode::ReturnToMainMenu()
{
	if (CSGameInstance)
	{
		const FString ContextStr = TEXT("ReturnToMainMenu");
		if (const FLevelRow* Row = CSGameInstance->LevelData->FindRow<FLevelRow>(FName("MainMenuLevel"), ContextStr))
		{
			if (!Row->MapPath.IsEmpty())
			{
				UGameplayStatics::OpenLevel(this, FName(*Row->MapPath));
			}
		}
	}
}

void ACSSingleGameMode::UpdateGameStateAIStatus()
{
	if (SingleGameState)
	{
		SingleGameState->SetTotalAICount(TotalAICount);
		SingleGameState->SetDeadAICount(DeadAICount);
	}
}
