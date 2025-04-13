#include "GameModes/CSSingleGameMode.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSSingleGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "Data/CSLevelRow.h"
#include "Actor/CSBossGate.h"
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

	InitGameLogic();
}

void ACSSingleGameMode::InitGameLogic()
{
	CountInitialAI();
	UpdateGameStateAIStatus();

	InitTagPlayer();
	InitBossGateActor();

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
			ActivateBossGate();
			StartBossPhase();
		}
	}
}

ESpawnSlotType ACSSingleGameMode::GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState)
{
	return ESpawnSlotType::Single_Player_Slot0;
}

void ACSSingleGameMode::InitTagPlayer()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = It->Get())
		{
			if (APawn* Pawn = PlayerController->GetPawn())
			{
				Pawn->Tags.AddUnique("SinglePlayer");
			}
		}
	}
}

void ACSSingleGameMode::InitBossGateActor()
{
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(this, ACSBossGate::StaticClass(), Found);

	if (Found.Num() > 0)
	{
		BossGateActor = Cast<ACSBossGate>(Found[0]);
	}
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

void ACSSingleGameMode::ActivateBossGate()
{
	if (BossGateActor)
	{
		BossGateActor->Activate();
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
	UE_LOG(LogTemp, Warning, TEXT("보스 스폰함수 호출됨"));
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
	if (CSGameInstance)
	{
		const FLevelRow* LevelRow = CSGameInstance->FindLevelRow(FName("SingleModeLevel"));
		if (!LevelRow || LevelRow->MapPath.IsEmpty()) return;

		UGameplayStatics::OpenLevel(this, FName(*LevelRow->MapPath));
	}
}

void ACSSingleGameMode::ReturnToMainMenu()
{
	if (CSGameInstance)
	{
		const FLevelRow* LevelRow = CSGameInstance->FindLevelRow(FName("MainMenuLevel"));
		if (!LevelRow || LevelRow->MapPath.IsEmpty()) return;

		UGameplayStatics::OpenLevel(this, FName(*LevelRow->MapPath));
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
