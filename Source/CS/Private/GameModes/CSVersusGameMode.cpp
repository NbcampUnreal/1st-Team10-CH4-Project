#include "GameModes/CSVersusGameMode.h"
//#include "GameStates/CSVersusGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Controller/CSPlayerController.h"
#include "Managers/CSSpawnManager.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "TimerManager.h"

ACSVersusGameMode::ACSVersusGameMode()
{
    PrimaryActorTick.bCanEverTick = true;

    MatchTimeLimit = 90.0f;
    ElapsedTime = 0.0f;
    bIsSuddenDeath = false;

    /*GameStateClass = ACSVersusGameState::StaticClass();*/
}

void ACSVersusGameMode::BeginPlay()
{
    Super::BeginPlay();
}

void ACSVersusGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (MatchPhase == EMatchPhase::EMP_Playing)
    {
        ElapsedTime += DeltaSeconds;

        if (!bIsSuddenDeath && ElapsedTime >= MatchTimeLimit)
        {
            TriggerSuddenDeath();
        }
    }
}

void ACSVersusGameMode::InitVersusLogic()
{
    bIsSuddenDeath = false;
    ElapsedTime = 0.0f;

    SpawnPlayerAtTeamSlots();
    StartMatchCountDown();
}

void ACSVersusGameMode::SpawnPlayerAtTeamSlots()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSSpawnManager::StaticClass(), FoundActors);

	TArray<ACSSpawnManager*> SpawnManagers;
	for (AActor* Actor : FoundActors)
	{
		if (ACSSpawnManager* SM = Cast<ACSSpawnManager>(Actor))
		{
			SpawnManagers.Add(SM);
		}
	}

	TMap<int32, TArray<ESpawnSlotType>> TeamSlotPriority;
	TeamSlotPriority.Add(0, { ESpawnSlotType::Versus_Team0_Slot0, ESpawnSlotType::Versus_Team0_Slot1, ESpawnSlotType::Versus_Team0_Slot2 });
	TeamSlotPriority.Add(1, { ESpawnSlotType::Versus_Team1_Slot0, ESpawnSlotType::Versus_Team1_Slot1, ESpawnSlotType::Versus_Team1_Slot2 });

	TMap<int32, TArray<ACSPlayerState*>> TeamMap;
	/*for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPS = Cast<ACSPlayerState>(PlayerState))
		{
			TeamMap.FindOrAdd(CSPS->TeamID).Add(CSPS);
		}
	}*/

	for (auto& TeamEntry : TeamMap)
	{
		int32 TeamID = TeamEntry.Key;
		TArray<ACSPlayerState*>& Players = TeamEntry.Value;
		int32 MaxSlot = FMath::Min(Players.Num(), TeamSlotPriority[TeamID].Num());

		for (int32 i = 0; i < MaxSlot; ++i)
		{
			ESpawnSlotType SlotType = TeamSlotPriority[TeamID][i];

			for (ACSSpawnManager* SM : SpawnManagers)
			{
				if (SM->SlotType == SlotType)
				{
					if (AController* PC = Cast<AController>(Players[i]->GetOwner()))
					{
						// TODO: 캐릭터 ID로 클래스 가져오기
						FVector SpawnLoc = SM->GetActorLocation();
						FRotator SpawnRot = SM->GetActorRotation();

						// 임시 캐릭터 클래스 - 실제로는 DataTable 참조 필요
						TSubclassOf<APawn> CharacterClass = DefaultPawnClass;
						APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(CharacterClass, SpawnLoc, SpawnRot);
						if (SpawnedPawn)
						{
							PC->Possess(SpawnedPawn);
						}
					}
					break;
				}
			}
		}
	}
}

void ACSVersusGameMode::StartMatchCountDown()
{
	HandleStartGame();
	// TODO: 클라에 카운트다운 UI 표시 요청
}

void ACSVersusGameMode::HandlePlayerDeath(AController* DeadPlayer)
{
	UpdateAliveTeams();
	CheckWinCondition();
}

void ACSVersusGameMode::UpdateAliveTeams()
{
	/*AliveTeam0 = 0;
	AliveTeam1 = 0;

	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPS = Cast<ACSPlayerState>(PS))
		{
			if (AController* PC = Cast<AController>(CSPS->GetOwner()))
			{
				if (APawn* Pawn = PC->GetPawn())
				{
					if (!Pawn->IsPendingKill())
					{
						(CSPS->TeamID == 0) ? AliveTeam0++ : AliveTeam1++;
					}
				}
			}
		}
	}*/
}

void ACSVersusGameMode::CheckWinCondition()
{
	if (AliveTeam0 == 0 && AliveTeam1 > 0)
	{
		FinishMatch(1);
	}
	else if (AliveTeam1 == 0 && AliveTeam0 > 0)
	{
		FinishMatch(0);
	}
}

void ACSVersusGameMode::TriggerSuddenDeath()
{
	bIsSuddenDeath = true;
	// TODO: 클라에 서든데스 진입 UI/연출 요청
}

void ACSVersusGameMode::FinishMatch(int32 WinningTeamID)
{
	HandleEndGame();
	// TODO: 승리 연출 (카메라 전환, UI 등)

	GetWorld()->GetTimerManager().SetTimer(ReturnToLobbyHandle, this, &ACSVersusGameMode::ReturnToLobby, 10.0f, false);
}

void ACSVersusGameMode::ReturnToLobby()
{
	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel(TEXT("/Game/Maps/Lobby?listen"));
}

