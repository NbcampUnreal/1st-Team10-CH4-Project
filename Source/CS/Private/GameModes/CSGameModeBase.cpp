#include "GameModes/CSGameModeBase.h"
#include "GameStates/CSGameStateBase.h"
#include "GameInstance/CSGameInstance.h"
#include "PlayerStates/CSPlayerState.h"
#include "Managers/CSSpawnManager.h"
#include "Kismet/GameplayStatics.h"

ACSGameModeBase::ACSGameModeBase()
{
	DefaultPawnClass = nullptr;

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

	if (LoggedInPlayerCount == ExpectedPlayerCount)
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
}

void ACSGameModeBase::SetAllPlayerInputEnabled(bool bEnabled)
{
	for (APlayerState* PlayerState : BaseGameState->PlayerArray)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(PlayerState->GetOwner()))
		{
			if (APawn* Pawn = PlayerController->GetPawn())
			{
				bEnabled ? Pawn->EnableInput(PlayerController) : Pawn->DisableInput(PlayerController);
			}
		}
	}
}

void ACSGameModeBase::SetMatchPhase(EMatchPhase NewPhase)
{
	MatchPhase = NewPhase;

	if (BaseGameState)
	{
		BaseGameState->MatchPhase = NewPhase;
	}
}

void ACSGameModeBase::SpawnAllPlayers()
{
	const TMap<ESpawnSlotType, ACSSpawnManager*> SlotMap = FindAllSpawnManager();
	if (SlotMap.IsEmpty()) return;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			ESpawnSlotType SlotType = GetSpawnSlotForPlayer(CSPlayerState);

			if (ACSSpawnManager* SpawnPoint = SlotMap.FindRef(SlotType))
			{
				if (APlayerController* PlayerController = Cast<APlayerController>(CSPlayerState->GetOwner()))
				{
					const FCharacterRow* Row = CSGameInstance->CharacterData->FindRow<FCharacterRow>(CSPlayerState->SelectedCharacterID, TEXT("SpawnPlayer"));

					if (!Row || !Row->CharacterClass.IsValid()) continue;

					TSubclassOf<APawn> CharacterClass = Row->CharacterClass.LoadSynchronous();
					APawn* Spawned = GetWorld()->SpawnActor<APawn>(CharacterClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation());

					if (Spawned)
					{
						PlayerController->Possess(Spawned);
						Spawned->DisableInput(PlayerController);
					}
				}
			}
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
		BaseGameState->RemainingMatchTime = MatchTimeLimit;
	}

	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ACSGameModeBase::UpdateMatchTimer, 1.0f, true);
}

void ACSGameModeBase::UpdateMatchTimer()
{
	if (BaseGameState)
	{
		BaseGameState->RemainingMatchTime--;

		if (BaseGameState->RemainingMatchTime <= 0)
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

	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel(TEXT("/Game/Maps/Lobby?listen"));
}

