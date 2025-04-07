#include "GameModes/CSTutorialGameMode.h"
#include "GameInstance/CSGameInstance.h" // GameInstance 방식 사용 시 필요
#include "PlayerStates/CSPlayerState.h" // PlayerState 사용 위해 헤더 추가
#include "Controller/CSPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameStates/CSTutorialGameState.h"
#include "TimerManager.h"
// #include "SpawnManager/CSSpawnManager.h" // FindAllSpawnManager 결과 사용 시 필요 (주석 유지)

ACSTutorialGameMode::ACSTutorialGameMode()
{
	// 기본 Pawn 클래스는 GetDefaultPawnClassForController 에서 결정.
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ACSPlayerController::StaticClass();
	GameStateClass = ACSTutorialGameState::StaticClass();
	PlayerStateClass = ACSPlayerState::StaticClass();

	ExpectedPlayerCount = 1;
	LoggedInPlayerCount = 0; // 초기화

	MatchTimeLimit = 0; // 0 또는 음수면 시간 제한 없음

	// Fallback 설정 필요.
}

void ACSTutorialGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer); // Base 클래스 로직 호출 (LoggedInPlayerCount 증가 등)

	// 모든 예상 플레이어가 접속했는지 확인.
	// ReadyToStartMatch() 함수가 true를 반환하면 엔진이 HandleMatchHasStarted()를 호출해 줄 것임.
	// 따라서 여기서 직접 HandleMatchHasStarted를 호출할 필요는 없음.
	UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 플레이어 %s 로그인. 현재 %d/%d 명 접속."), *NewPlayer->GetName(), LoggedInPlayerCount, ExpectedPlayerCount);
}

bool ACSTutorialGameMode::ReadyToStartMatch_Implementation()
{
	// 싱글 플레이어 튜토리얼이므로 1명 이상 접속하면 시작 가능.
	return LoggedInPlayerCount >= ExpectedPlayerCount;
}

void ACSTutorialGameMode::HandleMatchHasStarted()
{
	// 이 함수는 ReadyToStartMatch()가 true를 반환한 후 엔진에 의해 호출됨.
	UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 매치 시작! (HandleMatchHasStarted)"));

	// GameState 가져오기 (캐스팅)
	BaseGameState = GetGameState<ACSGameStateBase>(); // BaseGameState 멤버 변수에 저장
	if (!BaseGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] BaseGameState 가져오기 실패!"));
		return;
	}

	// 1. 게임 로직 초기화 (순수 가상 함수 구현 호출)
	InitGameLogic();

	// 2. 모든 플레이어 스폰 (베이스 클래스 기능 활용)
	SpawnAllPlayers(); // 내부적으로 HandleStartingNewPlayer 또는 RestartPlayer 호출

	// 3. 게임 시작 처리 (Phase 변경 및 입력 활성화 - 베이스 클래스 기능 활용)
	Super::HandleStartGame(); // 부모의 HandleStartGame 호출 (MatchPhase 변경, 입력 활성화 등)

	// 4. 매치 타이머 시작 (튜토리얼에서 필요하다면)
	if (MatchTimeLimit > 0)
	{
		StartMatchTimer();
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 매치 타이머 %d초 시작."), MatchTimeLimit);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 시간 제한 없음."));
	}
}


// --- PURE_VIRTUAL 함수 구현 ---

void ACSTutorialGameMode::InitGameLogic()
{
	// 이 함수는 HandleMatchHasStarted 내부에서 호출됨.
	UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 튜토리얼 게임 로직 초기화 시작 (InitGameLogic)."));

	// TODO: (GameState) 튜토리얼 목표 설정 등 초기화 로직 필요 시 구현.
	// ACSTutorialGameState* TGS = Cast<ACSTutorialGameState>(BaseGameState);
	// if (TGS) { /* TGS->SetupTutorialObjectives(); */ }
}

void ACSTutorialGameMode::HandlePlayerDeath(AController* DeadPlayer)
{
	// 이 함수는 플레이어 Pawn 등에서 사망 이벤트 발생 시 호출되어야 함. (보통 Pawn의 Die 함수 등에서 GameMode->HandlePlayerDeath 호출)
	// 튜토리얼에서는 플레이어가 죽지 않으므로, 실제 사망 처리는 하지 않음.
	if (DeadPlayer)
	{
		APawn* DeadPawn = DeadPlayer->GetPawn();
		UE_LOG(LogTemp, Warning, TEXT("[TutorialGM] 플레이어 %s 사망 처리 요청됨 (Pawn: %s). 튜토리얼이므로 무시하고 무적 상태 재확인."),
			*DeadPlayer->GetName(), DeadPawn ? *DeadPawn->GetName() : TEXT("None"));

		// 혹시 모르니 무적 상태 다시 한번 걸어주기 (필수는 아님)
		MakePlayerInvulnerable(DeadPlayer);

		// 필요하다면 시작 지점 등으로 강제 이동시키는 로직 추가 가능.
	}
}

// --- virtual 함수 오버라이드 ---

ESpawnSlotType ACSTutorialGameMode::GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState) const
{
	return ESpawnSlotType::Coop_Player_Slot0;
}

UClass* ACSTutorialGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	FName SelectedCharacterID = NAME_None;
	ACSPlayerState* PS = InController ? InController->GetPlayerState<ACSPlayerState>() : nullptr;

	// 주석 해제됨: PlayerState에서 SelectedCharacterID (FName) 읽기 시도
	if (PS && !PS->SelectedCharacterID.IsNone())
	{
		SelectedCharacterID = PS->SelectedCharacterID;
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] PlayerState에서 %s 의 선택 캐릭터 ID [%s] 가져옴."), *InController->GetName(), *SelectedCharacterID.ToString());
	}

	// --- FName ID로 Pawn 클래스 찾는 로직 (주석 유지) ---
// 주석 유지: 이 부분은 FName을 실제 Pawn 클래스로 변환하는 데이터 테이블 조회 로직 구현 필요.
	if (!SelectedCharacterID.IsNone())
	{
		// TODO: 데이터 테이블 조회 로직 구현
		// UDataTable* CharacterDataTable = ... ;
		// FCharacterData* Row = CharacterDataTable->FindRow<FCharacterData>(SelectedCharacterID, "");
		// if (Row && Row->CharacterClass) { return Row->CharacterClass; }
	}
	// --- Pawn 클래스 찾는 로직 끝 ---

	UE_LOG(LogTemp, Warning, TEXT("[TutorialGM] %s 의 선택 캐릭터 클래스 정보 못 찾거나 변환 실패. Fallback 사용."), *InController->GetName());
	return FallbackDefaultPawnClass;
}

void ACSTutorialGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// 베이스 클래스의 플레이어 스폰 로직 전에 Pawn 클래스 결정해야 함.
	// (GetDefaultPawnClassForController 가 이미 호출되었을 것임)

	// 베이스 클래스가 스폰 및 Possess 처리하도록 호출 (선택적).
	// Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	// 또는 직접 스폰/Possess 로직 구현 (이전 답변 참고) 후 무적 처리.

	// 여기서는 직접 스폰/Possess 후 무적 처리하는 방식으로 유지.
	if (!NewPlayer) return;
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (!PawnClass) PawnClass = FallbackDefaultPawnClass;
	if (!PawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] 스폰할 Pawn 클래스 없음!"));
		return;
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewPlayer;
	SpawnParams.Instigator = NewPlayer->GetInstigator();
	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParams);

	if (SpawnedPawn)
	{
		NewPlayer->Possess(SpawnedPawn);
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] %s (%s) 스폰 및 Possess 완료."), *SpawnedPawn->GetName(), *PawnClass->GetName());
		MakePlayerInvulnerable(NewPlayer); // 무적 처리
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] %s Pawn 스폰 실패!"), *PawnClass->GetName());
	}
}


void ACSTutorialGameMode::RestartPlayer(AController* NewPlayer)
{
	// 베이스 클래스 로직 호출 전에 Pawn 클래스 결정.
	// Super::RestartPlayer(NewPlayer);
	// 또는 직접 리스폰 로직 구현 후 무적 처리.

	// 여기서는 직접 리스폰/Possess 후 무적 처리하는 방식으로 유지.
	if (!NewPlayer) return;
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (!PawnClass) PawnClass = FallbackDefaultPawnClass;
	if (!PawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] 리스폰할 Pawn 클래스 없음!"));
		return;
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewPlayer;
	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParams);

	if (SpawnedPawn)
	{
		NewPlayer->UnPossess(); // 이전 빙의 해제
		NewPlayer->Possess(SpawnedPawn);
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] %s (%s) 리스폰 및 Possess 완료."), *SpawnedPawn->GetName(), *PawnClass->GetName());
		MakePlayerInvulnerable(NewPlayer); // 무적 처리
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] %s Pawn 리스폰 실패!"), *PawnClass->GetName());
	}
}

// --- 튜토리얼 자체 기능 구현 ---

void ACSTutorialGameMode::PlayerEnteredStageTransitionZone(APlayerController* Player)
{
	if (!Player) return;

	UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 플레이어 %s 스테이지 이동 구역 진입. 레벨 %s 로 이동."), *Player->GetName(), *StageLevelName.ToString());

	// 게임 종료 처리 (Phase 변경, 입력 비활성화 등 - 베이스 기능 활용)
	HandleEndGame(); // 게임 로직 정지

	// 다음 스테이지 레벨 로드
	UGameplayStatics::OpenLevel(this, StageLevelName);

	// 참고: 로비 복귀가 필요하면 Base 클래스의 ReturnToLobby() 함수 활용 가능.
	// GetWorldTimerManager().SetTimer(ReturnToLobbyHandle, this, &ACSGameModeBase::ReturnToLobby, 5.0f, false); // 예: 5초 후 로비 복귀
}

void ACSTutorialGameMode::MakePlayerInvulnerable(AController* PlayerController)
{
	if (!PlayerController) return;
	APawn* ControlledPawn = PlayerController->GetPawn();
	if (ControlledPawn)
	{
		ControlledPawn->SetCanBeDamaged(false);
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] %s (%s) 무적 설정 완료."), *PlayerController->GetName(), *ControlledPawn->GetName());
	}
}