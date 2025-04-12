#include "GameModes/CSGameModeBase.h"
#include "GameStates/CSGameStateBase.h"
#include "GameInstance/CSGameInstance.h"
#include "PlayerStates/CSPlayerState.h"
#include "AI/Controller/AIBaseController.h"
#include "Managers/CSSpawnManager.h"
#include "Data/CSLevelRow.h"
#include "Data/CSCharacterRow.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h" // APlayerController 사용 위해 포함
#include "GameFramework/Pawn.h"            // APawn 사용 위해 포함
#include "TimerManager.h"                // FTimerManager 사용 위해 포함

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
	// BaseGameState 캐스팅 확인
	BaseGameState = GetGameState<ACSGameStateBase>();
	if (!BaseGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("ACSGameModeBase::BeginPlay: Failed to get/cast BaseGameState!"));
	}


	SetMatchPhase(EMatchPhase::EMP_Waiting);

	if (CSGameInstance)
	{
		MatchType = CSGameInstance->GetMatchType();
		ExpectedPlayerCount = CSGameInstance->ExpectedPlayerCount;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ACSGameModeBase::BeginPlay: CSGameInstance is NULL!"));
	}
}

void ACSGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Log, TEXT("ACSGameModeBase::PostLogin --- Called for %s"), *NewPlayer->GetName());

	LoggedInPlayerCount++;
	UE_LOG(LogTemp, Log, TEXT("ACSGameModeBase::PostLogin --- LoggedInPlayerCount: %d, ExpectedPlayerCount: %d"), LoggedInPlayerCount, ExpectedPlayerCount);

	// 예상 플레이어 수 도달 시 게임 로직 초기화 (InitGameLogic은 파생 클래스에서 구현)
	if (LoggedInPlayerCount >= ExpectedPlayerCount && ExpectedPlayerCount > 0) // >= 로 변경, 0명 예상 제외
	{
		UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::PostLogin --- Expected player count reached. Calling InitGameLogic..."));
		InitGameLogic();
	}
}

void ACSGameModeBase::HandleStartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::HandleStartGame --- Called. Enabling player input..."));
	SetMatchPhase(EMatchPhase::EMP_Playing);
	SetAllPlayerInputEnabled(true); // 입력 활성화 요청
	StartMatchTimer();
}

void ACSGameModeBase::HandleEndGame()
{
	UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::HandleEndGame --- Called. Disabling player input..."));
	SetMatchPhase(EMatchPhase::EMP_Finished);
	SetAllPlayerInputEnabled(false); // 입력 비활성화 요청
}

void ACSGameModeBase::SetAllPlayerInputEnabled(bool bEnabled)
{
	UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::SetAllPlayerInputEnabled --- Called with bEnabled = %s"), bEnabled ? TEXT("true") : TEXT("false"));

	if (!BaseGameState) {
		UE_LOG(LogTemp, Error, TEXT("SetAllPlayerInputEnabled: BaseGameState is NULL! Cannot proceed."));
		return;
	}
	if (BaseGameState->PlayerArray.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("SetAllPlayerInputEnabled: PlayerArray is Empty."));
		return;
	}

	for (APlayerState* PlayerState : BaseGameState->PlayerArray) {
		if (!PlayerState) {
			UE_LOG(LogTemp, Warning, TEXT("SetAllPlayerInputEnabled: Found NULL PlayerState in PlayerArray. Skipping."));
			continue;
		}

		UE_LOG(LogTemp, Log, TEXT("SetAllPlayerInputEnabled: Processing PlayerState for %s"), *PlayerState->GetPlayerName());

		APlayerController* PlayerController = PlayerState->GetPlayerController(); // 직접 가져오기 시도
		if (!PlayerController) {
			UE_LOG(LogTemp, Warning, TEXT("SetAllPlayerInputEnabled: PlayerState->GetPlayerController() returned NULL for %s. Trying GetOwner cast..."), *PlayerState->GetPlayerName());
			PlayerController = Cast<APlayerController>(PlayerState->GetOwner()); // Fallback
		}

		if (PlayerController) {
			UE_LOG(LogTemp, Log, TEXT("SetAllPlayerInputEnabled: Found PlayerController: %s"), *PlayerController->GetName());
			APawn* Pawn = PlayerController->GetPawn();
			if (Pawn) {
				UE_LOG(LogTemp, Log, TEXT("SetAllPlayerInputEnabled: Found Pawn: %s"), *Pawn->GetName());
				if (bEnabled) {
					UE_LOG(LogTemp, Log, TEXT("SetAllPlayerInputEnabled: --> Enabling input for Pawn %s..."), *Pawn->GetName());
					Pawn->EnableInput(PlayerController);
					UE_LOG(LogTemp, Warning, TEXT("SetAllPlayerInputEnabled: --> Called EnableInput for Pawn %s."), *Pawn->GetName()); // Warning으로 강조
				}
				else {
					UE_LOG(LogTemp, Log, TEXT("SetAllPlayerInputEnabled: --> Disabling input for Pawn %s..."), *Pawn->GetName());
					Pawn->DisableInput(PlayerController);
					UE_LOG(LogTemp, Warning, TEXT("SetAllPlayerInputEnabled: --> Called DisableInput for Pawn %s."), *Pawn->GetName()); // Warning으로 강조
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("SetAllPlayerInputEnabled: Pawn is NULL for Controller %s! Cannot set input state."), *PlayerController->GetName());
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("SetAllPlayerInputEnabled: PlayerController is NULL for PlayerState %s! Cannot set input state."), *PlayerState->GetPlayerName());
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::SetAllPlayerInputEnabled --- Finished processing all players."));
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
	UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::SpawnAllPlayers --- Called")); // 로그 추가

	const TMap<ESpawnSlotType, ACSSpawnManager*> SlotMap = FindAllSpawnManager();
	if (SlotMap.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnAllPlayers: SlotMap is empty!"));
		return;
	}

	RestorePlayerLobbyData(); // 이 함수의 구현 내용 확인 필요

	if (!GameState) { UE_LOG(LogTemp, Error, TEXT("SpawnAllPlayers: GameState is NULL!")); return; }


	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (!PlayerState) continue;
		ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState);
		if (CSPlayerState)
		{
			UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: Processing player %s"), *CSPlayerState->GetPlayerName());
			ESpawnSlotType SlotType = GetSpawnSlotForPlayer(CSPlayerState); // 파생 클래스에서 구현된 함수 호출
			UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: SlotType for %s is %d"), *CSPlayerState->GetPlayerName(), static_cast<int32>(SlotType));

			ACSSpawnManager* SpawnPoint = SlotMap.FindRef(SlotType);
			if (SpawnPoint)
			{
				UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: Found SpawnPoint at %s"), *SpawnPoint->GetActorLocation().ToString());
				APlayerController* PlayerController = Cast<APlayerController>(CSPlayerState->GetOwner());
				if (PlayerController)
				{
					UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: Found Controller %s"), *PlayerController->GetName());
					if (!CSGameInstance) { UE_LOG(LogTemp, Error, TEXT("SpawnAllPlayers: CSGameInstance is NULL!")); continue; }

					const FCharacterRow* Row = CSGameInstance->FindCharacterRowByJob(CSPlayerState->SelectedJob);
					if (!Row) { UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: CharacterRow not found for Job %d"), (int32)CSPlayerState->SelectedJob); continue; }
					UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: Found CharacterRow for Job %d"), (int32)CSPlayerState->SelectedJob);

					TSubclassOf<APawn> CharacterClass = Row->CharacterClass.LoadSynchronous();
					if (!CharacterClass) { UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: CharacterClass is invalid in Row!")); continue; }
					UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: Loaded CharacterClass %s"), *CharacterClass->GetName());

					// 스폰 위치 로그 추가
					FVector SpawnLoc = SpawnPoint->GetActorLocation();
					FRotator SpawnRot = SpawnPoint->GetActorRotation();
					UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: Spawning %s at Loc=%s, Rot=%s"), *CharacterClass->GetName(), *SpawnLoc.ToString(), *SpawnRot.ToString());

					APawn* Spawned = GetWorld()->SpawnActor<APawn>(CharacterClass, SpawnLoc, SpawnRot);
					if (Spawned)
					{
						UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: Spawned Pawn %s successfully."), *Spawned->GetName());
						PlayerController->Possess(Spawned);
						// Possess 직후 확인
						if (Spawned->GetController() == PlayerController) {
							UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: Pawn %s possessed by %s."), *Spawned->GetName(), *PlayerController->GetName());
						}
						else {
							UE_LOG(LogTemp, Error, TEXT("SpawnAllPlayers: Pawn %s FAILED to possess!"), *Spawned->GetName());
						}

						// 입력 활성화 시도
						UE_LOG(LogTemp, Log, TEXT("SpawnAllPlayers: Enabling input for spawned Pawn %s..."), *Spawned->GetName());
						Spawned->EnableInput(PlayerController); // 여기서 먼저 활성화 시도
						UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: Called EnableInput for spawned Pawn %s."), *Spawned->GetName());
					}
					else { UE_LOG(LogTemp, Error, TEXT("SpawnAllPlayers: Failed to spawn Pawn!")); }
				}
				else { UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: PlayerController is NULL for PlayerState %s."), *CSPlayerState->GetPlayerName()); }
			}
			else { UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: SpawnPoint is NULL for SlotType %d."), static_cast<int32>(SlotType)); }
		}
		else { UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: Failed to cast PlayerState to ACSPlayerState.")); }
	}
	UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::SpawnAllPlayers --- Finished"));
}

void ACSGameModeBase::RestorePlayerLobbyData()
{
	UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::RestorePlayerLobbyData --- Called (Implementation needed if used)."));

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

