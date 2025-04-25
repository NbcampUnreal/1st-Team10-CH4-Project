#include "GameModes/CSGameModeBase.h"
#include "GameStates/CSGameStateBase.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "PlayerStates/CSPlayerState.h"
#include "AI/Controller/AIBaseController.h"
#include "Managers/CSSpawnManager.h"
#include "Data/CSLevelRow.h"
#include "Data/CSCharacterRow.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h" 
#include "GameFramework/Pawn.h"           
#include "TimerManager.h"           
#include "Controller/CSPlayerController.h"
#include "UI/CSUIBaseWidget.h"   
#include "Framework/Application/SlateApplication.h" 

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

	CSGameInstance = GetGameInstance<UCSAdvancedGameInstance>();
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

void ACSGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	APlayerController* PlayerController = Cast<APlayerController>(C);
	if (!PlayerController) return;

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
	if (MatchPhase == NewPhase) return;

	MatchPhase = NewPhase;

	if (BaseGameState) 
	{
		BaseGameState->SetMatchPhase(NewPhase);
	}

	if (HasAuthority()) 
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (PlayerController && PlayerController->IsLocalPlayerController())
		{
			ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(PlayerController);
			if (CSPlayerController)
			{
				UCSUIBaseWidget* HostUI = CSPlayerController->GetCurrentUI();
				if (HostUI)
				{
					HostUI->HandleMatchPhaseChanged(NewPhase);

					if (NewPhase == EMatchPhase::EMP_Playing) 
					{
						FInputModeGameOnly InputModeData;
						CSPlayerController->SetInputMode(InputModeData);
						CSPlayerController->bShowMouseCursor = false;
						FSlateApplication::Get().SetUserFocusToGameViewport(0);
					}
					else 
					{ 
						FInputModeUIOnly InputModeData;
						CSPlayerController->SetInputMode(InputModeData);
						CSPlayerController->bShowMouseCursor = true;
					}
				}
			}
		}
	}
}

AController* ACSGameModeBase::FindAliveTeammate(AController* DeadPlayer)
{
	if (!DeadPlayer) return nullptr;

	ACSPlayerState* DeadState = DeadPlayer->GetPlayerState<ACSPlayerState>();
	if (!DeadState) return nullptr;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState);
		if (!CSPlayerState) continue;

		if (CSPlayerState->TeamID == DeadState->TeamID && CSPlayerState->bIsAlive && CSPlayerState != DeadState)
		{
			return Cast<AController>(CSPlayerState->GetOwningController());
		}
	}

	return nullptr;
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
				if (!PlayerController) continue;

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
						
						if (PlayerController->IsLocalController())
						{
							Spawned->EnableInput(PlayerController);
							APawn* Pawn = PlayerController->GetPawn();
						}
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

void ACSGameModeBase::ReturnToMainMenu(AController* TargetPlayer)
{
	if (!CSGameInstance) return;

	const FLevelRow* LevelRow = CSGameInstance->FindLevelRow(FName("MainMenuLevel"));
	if (!LevelRow || LevelRow->MapPath.IsEmpty()) return;

	const FString TravelURL = LevelRow->MapPath;


	// 싱글플레이
	if (GetNetMode() == NM_Standalone)
	{
		UGameplayStatics::OpenLevel(this, FName(*TravelURL));
		return;
	}

	// 호스트가 전체 이동
	if (HasAuthority() && !TargetPlayer)
	{
		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session.IsValid()) Session->DestroySession(NAME_GameSession);

		GetWorld()->ServerTravel(TravelURL);
		return;
	}

	// 특정 플레이어(게스트 한 명) 이동
	if (TargetPlayer)
	{
		APlayerController* PlayerController = Cast<APlayerController>(TargetPlayer);
		if (!PlayerController) return;

		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session.IsValid()) Session->DestroySession(NAME_GameSession);

		PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
}

void ACSGameModeBase::ReturnAllPlayersToMainMenu()
{
	ReturnToMainMenu(nullptr);
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

