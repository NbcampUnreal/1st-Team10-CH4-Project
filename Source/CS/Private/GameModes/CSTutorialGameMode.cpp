#include "GameModes/CSTutorialGameMode.h"
#include "GameInstance/CSGameInstance.h"     // 캐릭터 선택 정보 가져오기 (주석 유지)
// #include "PlayerStates/CSPlayerState.h"    // 캐릭터 선택 정보 가져오기 (주석 유지)
#include "Controller/CSPlayerController.h" // 캐스팅
#include "GameFramework/Pawn.h"              // Pawn 클래스 및 SetCanBeDamaged 사용
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameStates/CSTutorialGameState.h" // 헤더 포함

ACSTutorialGameMode::ACSTutorialGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ACSPlayerController::StaticClass();
	 // 주석 해제됨: ACSTutorialGameState 클래스가 이제 존재하므로 연결 가능.
		GameStateClass = ACSTutorialGameState::StaticClass();
	 // 주석 유지: ACSPlayerState 라는 이름의 커스텀 PlayerState 클래스가 실제로 존재하고 필요한 기능이 구현되었을 때 해제해야 함.
		// PlayerStateClass = ACSPlayerState::StaticClass();

		// Fallback 설정 등...
}

void ACSTutorialGameMode::InitSinglePlayLogic()
{
	Super::InitSinglePlayLogic();
	UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 튜토리얼 모드 로직 초기화중."));

	 // 주석 유지: ACSTutorialGameState에 실제 튜토리얼 목표 설정/관리 함수가 구현되어야 호출 가능함.
		// TODO: (GameState) 튜토리얼 목표 설정 등 초기화 로직 필요 시 구현.
		// 예: ACSTutorialGameState* TGS = GetGameState<ACSTutorialGameState>(); if (TGS) { TGS->SetupTutorialObjectives(); }
}

void ACSTutorialGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	if (!NewPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TutorialGM] HandleStartingNewPlayer 호출됐는데 NewPlayer가 없음."));
		return;
	}

	// 캐릭터 선택 정보를 기반으로 Pawn 클래스를 결정 (내부 로직은 아직 주석 처리 상태).
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (!PawnClass || PawnClass == FallbackDefaultPawnClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TutorialGM] %s 의 Pawn 클래스를 선택 정보로 결정 못함. Fallback 사용."), *NewPlayer->GetName());
		PawnClass = FallbackDefaultPawnClass;
	}

	if (!PawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] FallbackDefaultPawnClass 설정 안됨! %s 의 Pawn 스폰 불가."), *NewPlayer->GetName());
		return;
	}

	// 스폰 위치 찾고 Pawn 스폰.
	AActor* StartSpot = FindPlayerStart(NewPlayer);
	FVector SpawnLocation = StartSpot ? StartSpot->GetActorLocation() : FVector::ZeroVector;
	FRotator SpawnRotation = StartSpot ? StartSpot->GetActorRotation() : FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewPlayer;
	SpawnParams.Instigator = NewPlayer->GetInstigator();
	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedPawn)
	{
		NewPlayer->Possess(SpawnedPawn);
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] %s (%s) 스폰 완료, %s 가 Possess 함."), *SpawnedPawn->GetName(), *PawnClass->GetName(), *NewPlayer->GetName());

		// === 플레이어 무적 처리 ===
		MakePlayerInvulnerable(NewPlayer);
		// ========================
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] %s 의 Pawn (%s) 스폰 실패! 위치: %s"), *NewPlayer->GetName(), *PawnClass->GetName(), *SpawnLocation.ToString());
	}
}

void ACSTutorialGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!NewPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TutorialGM] RestartPlayer 호출됐는데 NewPlayer가 없음."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 플레이어 %s 리스타트 시도 (튜토리얼에서는 죽지 않아야 하지만 로직은 유지)."), *NewPlayer->GetName());

	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer);
	if (!PawnClass || PawnClass == FallbackDefaultPawnClass)
	{
		PawnClass = FallbackDefaultPawnClass;
	}

	if (!PawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] FallbackDefaultPawnClass 설정 안됨! %s 리스폰 불가."), *NewPlayer->GetName());
		return;
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer);
	FVector SpawnLocation = StartSpot ? StartSpot->GetActorLocation() : FVector::ZeroVector;
	FRotator SpawnRotation = StartSpot ? StartSpot->GetActorRotation() : FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewPlayer;
	// SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; // 충돌 처리 필요시 옵션 조절

	APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (SpawnedPawn)
	{
		// 기존 빙의 해제 (필수는 아닐 수 있지만 안전하게)
		NewPlayer->UnPossess();
		NewPlayer->Possess(SpawnedPawn);
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] %s (%s) 리스폰 완료, %s 가 Possess 함."), *SpawnedPawn->GetName(), *PawnClass->GetName(), *NewPlayer->GetName());

		// === 플레이어 무적 처리 ===
		MakePlayerInvulnerable(NewPlayer);
		// ========================
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[TutorialGM] %s 의 Pawn (%s) 리스폰 실패! 위치: %s"), *NewPlayer->GetName(), *PawnClass->GetName(), *SpawnLocation.ToString());
	}
}

void ACSTutorialGameMode::HandleMatchIsWaitingToStart()
{
	UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 매치 시작 대기중... 바로 Playing 상태로 전환함."));
	HandleStartGame(); // MatchPhase를 EMP_Playing으로 변경
}

UClass* ACSTutorialGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// --- 캐릭터 선택 정보 가져오는 로직 (주석 유지) ---
	 // 주석 유지: 아래 로직들은 CSGameInstance나 CSPlayerState에 'SelectedCharacterClass' 같은 변수가 실제로 정의되어야 사용 가능함.

		// 방법 1: GameInstance 사용 예시
		/*
		UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
		if (GI && GI->SelectedCharacterClass) // GI->SelectedCharacterClass 가 유효하다면
		{
			UE_LOG(LogTemp, Log, TEXT("[TutorialGM] GameInstance에서 %s 의 선택 캐릭터 클래스 가져옴."), *InController->GetName());
			return GI->SelectedCharacterClass;
		}
		*/

		// 방법 2: PlayerState 사용 예시
		/*
		ACSPlayerState* PS = InController ? InController->GetPlayerState<ACSPlayerState>() : nullptr; // ACSPlayerState로 캐스팅 필요
		if (PS && PS->SelectedCharacterClass) // PS->SelectedCharacterClass 가 유효하다면
		{
			UE_LOG(LogTemp, Log, TEXT("[TutorialGM] PlayerState에서 %s 의 선택 캐릭터 클래스 가져옴."), *InController->GetName());
			return PS->SelectedCharacterClass;
		}
		*/
		// --- 캐릭터 선택 정보 가져오는 로직 끝 ---

		UE_LOG(LogTemp, Warning, TEXT("[TutorialGM] %s 의 선택 캐릭터 클래스 정보 못 찾음. Fallback 사용."), *InController->GetName());
	return FallbackDefaultPawnClass; // 정보가 없으면 Fallback 반환
}

void ACSTutorialGameMode::PlayerEnteredStageTransitionZone(APlayerController* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TutorialGM] PlayerEnteredStageTransitionZone 호출됐지만 PlayerController가 없음."));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[TutorialGM] 플레이어 %s 가 스테이지 이동 구역에 진입함. 레벨 %s 로 이동 시작!"),
		*Player->GetName(),
		*StageLevelName.ToString());

	// 다음 스테이지 레벨로 이동
	UGameplayStatics::OpenLevel(this, StageLevelName);

	// 중요: 이 함수는 레벨에 배치된 TriggerVolume 같은 액터가
	// OnActorBeginOverlap 이벤트에서 플레이어 Pawn을 감지했을 때 호출해줘야 함.
	// 예시 (트리거 블루프린트 로직):
	// 1. OnActorBeginOverlap 이벤트 받기
	// 2. OtherActor를 플레이어 Pawn 클래스로 Cast 하기
	// 3. 성공 시, Pawn에서 GetController -> PlayerController로 Cast 하기
	// 4. 성공 시, Get Game Mode -> CSTutorialGameMode로 Cast 하기
	// 5. 성공 시, GameMode의 PlayerEnteredStageTransitionZone 함수 호출 (PlayerController 전달)
}

void ACSTutorialGameMode::MakePlayerInvulnerable(AController* PlayerController)
{
	if (!PlayerController) return;

	APawn* ControlledPawn = PlayerController->GetPawn();
	if (ControlledPawn)
	{
		// Pawn이 데미지를 받을 수 없도록 설정.
		ControlledPawn->SetCanBeDamaged(false);
		UE_LOG(LogTemp, Log, TEXT("[TutorialGM] %s (%s) 를 무적으로 설정함 (SetCanBeDamaged=false)."), *PlayerController->GetName(), *ControlledPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TutorialGM] %s 가 빙의한 Pawn이 없어서 무적 처리 못함."), *PlayerController->GetName());
	}
}