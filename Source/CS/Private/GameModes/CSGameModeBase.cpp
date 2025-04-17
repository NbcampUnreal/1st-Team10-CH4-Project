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
		UE_LOG(LogTemp, Warning, TEXT("Versus BeginPlay: MatchType = %d, ExpectedPlayerCount = %d"), (int32)MatchType, ExpectedPlayerCount);
	}
}

void ACSGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	LoggedInPlayerCount++;
	UE_LOG(LogTemp, Warning, TEXT("[Versus] PostLogin: LoggedInPlayerCount = %d"), LoggedInPlayerCount);
	if (LoggedInPlayerCount >= ExpectedPlayerCount && ExpectedPlayerCount > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Versus] PostLogin: LoggedInPlayerCount = %d / Expected = %d"), LoggedInPlayerCount, ExpectedPlayerCount);
		InitGameLogic();
	}
}

void ACSGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	APlayerController* PlayerController = Cast<APlayerController>(C);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("HandleSeamlessTravelPlayer: Cast 실패"));
		return;
	}

	LoggedInPlayerCount++;
	UE_LOG(LogTemp, Warning, TEXT("[HandleSeamlessTravelPlayer] LoggedInPlayerCount = %d"), LoggedInPlayerCount);

	if (LoggedInPlayerCount >= ExpectedPlayerCount && ExpectedPlayerCount > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HandleSeamlessTravelPlayer] ExpectedPlayerCount 도달 -> InitGameLogic 호출"));
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

	UE_LOG(LogTemp, Log, TEXT("ACSGameModeBase::SetMatchPhase: Changing Phase from %d to %d"), (int32)MatchPhase, (int32)NewPhase);
	MatchPhase = NewPhase;

	if (BaseGameState) {
		BaseGameState->SetMatchPhase(NewPhase); // 리플리케이트될 GameState 변수 설정
	}

	// --- 호스트(리슨 서버) UI 직접 업데이트 로직 추가 ---
	if (HasAuthority()) // 서버에서만 실행
	{
		// 호스트의 로컬 플레이어 컨트롤러 가져오기 (Index 0)
		APlayerController* HostPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (HostPC && HostPC->IsLocalPlayerController())
		{
			ACSPlayerController* HostCSPC = Cast<ACSPlayerController>(HostPC);
			if (HostCSPC)
			{
				UCSUIBaseWidget* HostUI = HostCSPC->GetCurrentUI();
				if (HostUI)
				{
					// 호스트 UI의 이벤트 직접 호출
					HostUI->HandleMatchPhaseChanged(NewPhase);
					UE_LOG(LogTemp, Log, TEXT("ACSGameModeBase::SetMatchPhase: Called HandleMatchPhaseChanged on Host UI."));

					// --- 호스트 입력 모드 설정 (여기서 하는 것이 더 안정적) ---
					if (NewPhase == EMatchPhase::EMP_Playing) {
						UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::SetMatchPhase: Setting Input Mode to GameOnly for Host"));
						FInputModeGameOnly InputModeData;
						HostCSPC->SetInputMode(InputModeData);
						HostCSPC->bShowMouseCursor = false;
						FSlateApplication::Get().SetUserFocusToGameViewport(0);
					}
					else {
						UE_LOG(LogTemp, Warning, TEXT("ACSGameModeBase::SetMatchPhase: Setting Input Mode to UIOnly for Host"));
						FInputModeUIOnly InputModeData;
						HostCSPC->SetInputMode(InputModeData);
						HostCSPC->bShowMouseCursor = true;
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
	UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: 호출됨"));
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

			UE_LOG(LogTemp, Warning, TEXT("SpawnAllPlayers: %s 스폰 시도 중"), *CSPlayerState->GetPlayerName());

			if (SpawnPoint) 
			{
				APlayerController* PlayerController = Cast<APlayerController>(CSPlayerState->GetOwner());
				if (!PlayerController)
				{
					UE_LOG(LogTemp, Error, TEXT("SpawnAllPlayers: PlayerController 없음"));
					continue;
				}
				if (PlayerController) 
				{
					if (!CSGameInstance) continue;
					const FCharacterRow* Row = CSGameInstance->FindCharacterRowByJob(CSPlayerState->SelectedJob);
					if (!Row)
					{
						UE_LOG(LogTemp, Error, TEXT("SpawnAllPlayers: CharacterRow 찾기 실패: JobType = %d"), (int32)CSPlayerState->SelectedJob);
						continue;
					}

					TSubclassOf<APawn> CharacterClass = Row->CharacterClass.LoadSynchronous();
					if (!CharacterClass)
					{
						UE_LOG(LogTemp, Error, TEXT("SpawnAllPlayers: CharacterClass 로드 실패"));
						continue;
					}

					FVector SpawnLoc = SpawnPoint->GetActorLocation();
					FRotator SpawnRot = SpawnPoint->GetActorRotation();

					APawn* Spawned = GetWorld()->SpawnActor<APawn>(CharacterClass, SpawnLoc, SpawnRot);
					if (Spawned) 
					{
						PlayerController->Possess(Spawned);
						UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("[GameMode] Possess 완료: %s | Controller: %s | IsLocallyControlled=%d | HasAuthority=%d"), * Spawned->GetName(), *GetNameSafe(PlayerController), Spawned->IsLocallyControlled(), Spawned->HasAuthority()), true, false, FLinearColor(1.0, 0.0, 0.0, 1), 30.0f);
						UE_LOG(LogTemp, Warning, TEXT("[GameMode] Possess 완료: %s | Controller: %s | IsLocallyControlled=%d | HasAuthority=%d"),
							*Spawned->GetName(), *GetNameSafe(PlayerController), Spawned->IsLocallyControlled(), Spawned->HasAuthority());
						if (Spawned->IsLocallyControlled())
						{
							UE_LOG(LogTemp, Warning, TEXT("Spawned Pawn IsLocallyControlled: TRUE"));
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Spawned Pawn IsLocallyControlled: FALSE"));
						}
						//if(PlayerController->IsLocalController())
						//{
						//	Spawned->AutoPossessPlayer = EAutoReceiveInput::Player0; // ★ 핵심 라인
						//}
						if (PlayerController->IsLocalController())
						{
							Spawned->EnableInput(PlayerController);
							APawn* Pawn = PlayerController->GetPawn();
							UE_LOG(LogTemp, Warning, TEXT("Possessed Pawn: %s"), Pawn ? *Pawn->GetName() : TEXT("NULL"));
							UE_LOG(LogTemp, Warning, TEXT("Expected Pawn: %s"), *Spawned->GetName());
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
		APlayerController* PC = Cast<APlayerController>(TargetPlayer);
		if (!PC) return;

		IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session.IsValid()) Session->DestroySession(NAME_GameSession);

		PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
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

