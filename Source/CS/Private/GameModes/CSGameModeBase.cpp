#include "GameModes/CSGameModeBase.h"
#include "GameStates/CSGameStateBase.h"
#include "GameInstance/CSGameInstance.h"
#include "PlayerStates/CSPlayerState.h"
#include "AI/Controller/AIBaseController.h"
#include "Managers/CSSpawnManager.h"
#include "Data/CSLevelRow.h"
#include "Data/CSCharacterRow.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h" 
#include "GameFramework/Pawn.h"           
#include "TimerManager.h"               

ACSGameModeBase::ACSGameModeBase()
{
	DefaultPawnClass = nullptr;
	PlayerStateClass = ACSPlayerState::StaticClass();

	CSGameInstance = nullptr;
	BaseGameState = nullptr;

	MatchType = EMatchType::EMT_None;
	MatchPhase = EMatchPhase::EMP_None;

	LoggedInPlayerCount = 0;
	ExpectedPlayerCount = 0;
}

void ACSGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	CSGameInstance = GetGameInstance<UCSGameInstance>();
	BaseGameState = GetGameState<ACSGameStateBase>();
	SetMatchPhase(EMatchPhase::EMP_Waiting);
	if (CSGameInstance)
	{
		MatchType = CSGameInstance->GetMatchType();
		ExpectedPlayerCount = CSGameInstance->ExpectedPlayerCount;
	}
}

void ACSGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	LoggedInPlayerCount++;
	if (LoggedInPlayerCount >= ExpectedPlayerCount && ExpectedPlayerCount > 0)
	{
		InitGameLogic();
	}
}

void ACSGameModeBase::HandleStartGame()
{
	SetMatchPhase(EMatchPhase::EMP_Playing);
	SetAllPlayerInputEnabled(true);
	StartMatchTimer();
}

void ACSGameModeBase::HandleEndGame()
{
	SetMatchPhase(EMatchPhase::EMP_Finished);
	SetAllPlayerInputEnabled(false);
	
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);
}

void ACSGameModeBase::SetAllPlayerInputEnabled(bool bEnabled)
{
	if (!BaseGameState) return;

	for (APlayerState* PlayerState : BaseGameState->PlayerArray) 
	{
		if (!PlayerState) continue;

		APlayerController* PlayerController = PlayerState->GetPlayerController();
		if (!PlayerController) PlayerController = Cast<APlayerController>(PlayerState->GetOwner());

		if (PlayerController) 
		{
			APawn* Pawn = PlayerController->GetPawn();
			if (Pawn) 
			{
				bEnabled ? Pawn->EnableInput(PlayerController) : Pawn->DisableInput(PlayerController);
			}
		}
	}
}

void ACSGameModeBase::AllAIStartLogic(const TArray<APawn*>& InAIPawns)
{
	for (APawn* AIPawn : InAIPawns) 
	{
		if (AIPawn && AIPawn->GetController()) 
		{
			if (AAIBaseController* AIController = Cast<AAIBaseController>(AIPawn->GetController())) 
			{
				AIController->StartLogicAI();
			}
		}
	}
}

void ACSGameModeBase::SetMatchPhase(EMatchPhase NewPhase)
{
	MatchPhase = NewPhase;

	if (BaseGameState)
	{
		BaseGameState->SetMatchPhase(NewPhase);
	}
}

void ACSGameModeBase::SpawnAllPlayers()
{
	const TMap<ESpawnSlotType, ACSSpawnManager*> SlotMap = FindAllSpawnManager();
	if (SlotMap.IsEmpty()) return;

	RestorePlayerLobbyData();

	if (!GameState) return;

	for (APlayerState* PlayerState : GameState->PlayerArray) 
	{
		if (!PlayerState) continue;
		ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState);

		if (CSPlayerState) 
		{
			ESpawnSlotType SlotType = GetSpawnSlotForPlayer(CSPlayerState);
			ACSSpawnManager* SpawnPoint = SlotMap.FindRef(SlotType);
			
			if (SpawnPoint) 
			{
				APlayerController* PlayerController = Cast<APlayerController>(CSPlayerState->GetOwner());
				if (PlayerController) 
				{
					if (!CSGameInstance) continue;
					const FCharacterRow* Row = CSGameInstance->FindCharacterRowByJob(CSPlayerState->SelectedJob);
					if (!Row) continue;

					TSubclassOf<APawn> CharacterClass = Row->CharacterClass.LoadSynchronous();
					if (!CharacterClass) continue;

					FVector SpawnLoc = SpawnPoint->GetActorLocation();
					FRotator SpawnRot = SpawnPoint->GetActorRotation();

					APawn* Spawned = GetWorld()->SpawnActor<APawn>(CharacterClass, SpawnLoc, SpawnRot);
					if (Spawned) 
					{
						PlayerController->Possess(Spawned);
						Spawned->EnableInput(PlayerController);
					}
				}
			}
		}
	}
}

void ACSGameModeBase::RestorePlayerLobbyData()
{
	if (!CSGameInstance) return;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			FPlayerLobbyData Data = CSGameInstance->GetPlayerLobbyData(CSPlayerState->GetPlayerName());

			CSPlayerState->SelectedJob = Data.SelectedJob;
			CSPlayerState->TeamID = Data.TeamID;
			CSPlayerState->PlayerIndex = Data.PlayerIndex;
		}
	}
}

TMap<ESpawnSlotType, ACSSpawnManager*> ACSGameModeBase::FindAllSpawnManager() const
{
	TMap<ESpawnSlotType, ACSSpawnManager*> Result;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSSpawnManager::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (ACSSpawnManager* SpawnManager = Cast<ACSSpawnManager>(Actor))
		{
			Result.Add(SpawnManager->SlotType, SpawnManager);
		}
	}

	return Result;
}

void ACSGameModeBase::StartMatchTimer()
{
	if (BaseGameState)
	{
		BaseGameState->SetRemainingMatchTime(MatchTimeLimit);
	}

	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ACSGameModeBase::UpdateMatchTimer, 1.0f, true);
}

void ACSGameModeBase::UpdateMatchTimer()
{
	if (BaseGameState)
	{
		int32 NewTime = BaseGameState->GetRemainingMatchTime();
		BaseGameState->SetRemainingMatchTime(NewTime - 1);
		

		if (BaseGameState->GetRemainingMatchTime() <= 0)
		{
			GetWorldTimerManager().ClearTimer(MatchTimerHandle);
		}
	}
}

void ACSGameModeBase::ReturnToLobby()
{
	if (GetWorldTimerManager().IsTimerActive(ReturnToLobbyHandle))
	{
		GetWorldTimerManager().ClearTimer(ReturnToLobbyHandle);
	}

	if (CSGameInstance)
	{
		CSGameInstance->ResetLobbySettings();
	}

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			CSPlayerState->ResetLobbySettings();
		}
	}

	if (CSGameInstance && CSGameInstance->LevelData)
	{
		const FLevelRow* LevelRow = CSGameInstance->FindLevelRow(FName("LobbyLevel"));
		if (!LevelRow || LevelRow->MapPath.IsEmpty()) return;

		const FString TravelURL = LevelRow->MapPath + TEXT("?listen");

		bUseSeamlessTravel = true;
		GetWorld()->ServerTravel(TravelURL);
	}
}

