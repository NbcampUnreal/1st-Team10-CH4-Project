#include "GameModes/CSTutorialGameMode.h"
#include "GameInstance/CSGameInstance.h"
// #include "GameFramework/PlayerState.h" // 아직 Custom PlayerState 없으므로 주석 처리
// #include "PlayerStates/CSPlayerState.h" // 만약 CSPlayerState가 있다면 이것을 사용
#include "Controller/CSPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
// #include "GameStates/CSTutorialGameState.h" // 아직 사용하지 않으므로 주석 처리 (필요시 해제)
#include "GameFramework/PlayerStart.h"

ACSTutorialGameMode::ACSTutorialGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ACSPlayerController::StaticClass();
	// GameStateClass = ACSTutorialGameState::StaticClass(); // CSTutorialGameState를 사용한다면 주석 해제
	// PlayerStateClass = ACSPlayerState::StaticClass(); // CSPlayerState를 사용한다면 주석 해제

	// FallbackDefaultPawnClass는 블루프린트에서 설정하거나, 여기에 직접 클래스를 지정
	// 임시: FallbackDefaultPawnClass = ADefaultPawn::StaticClass();
}

void ACSTutorialGameMode::InitSinglePlayLogic()
{
	Super::InitSinglePlayLogic();
	UE_LOG(LogTemp, Log, TEXT("[CSTutorialGameMode] Initializing Single Player Logic for Tutorial."));

	// TODO: (GameState) 튜토리얼 목표 설정, NPC 스폰 등 초기화 로직 필요 시 구현
	// 임시: ACSTutorialGameState* TGS = GetGameState<ACSTutorialGameState>(); if (TGS) { TGS->SetupTutorialObjectives(); }
}

void ACSTutorialGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	UE_LOG(LogTemp, Log, TEXT("[CSTutorialGameMode] Player %s logged in or traveled to Tutorial."), *NewPlayer->GetName());
	// 스폰 로직은 HandleStartingNewPlayer 에서 처리
}


void ACSTutorialGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// 캐릭터 선택 정보를 기반으로 Pawn 클래스를 결정
	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer); // 내부에서 GameInstance/PlayerState 접근 시도 (현재 주석 처리됨)
	if (!PawnClass || PawnClass == FallbackDefaultPawnClass) // 선택 정보를 못 가져왔거나 Fallback 이면 경고
	{
		UE_LOG(LogTemp, Warning, TEXT("[CSTutorialGameMode] Could not determine PawnClass based on selection for %s. Using fallback."), *NewPlayer->GetName());
		PawnClass = FallbackDefaultPawnClass; // Fallback 명시적 사용
	}

	if (!PawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[CSTutorialGameMode] FallbackDefaultPawnClass is not set! Cannot spawn Pawn for %s."), *NewPlayer->GetName());
		return; // 스폰할 Pawn 클래스가 없으면 중단
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer);
	FVector SpawnLocation = StartSpot ? StartSpot->GetActorLocation() : FVector::ZeroVector;
	FRotator SpawnRotation = StartSpot ? StartSpot->GetActorRotation() : FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewPlayer;
	SpawnParams.Instigator = NewPlayer->GetInstigator(); // Instigator 설정
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (NewPawn)
	{
		NewPlayer->Possess(NewPawn);
		UE_LOG(LogTemp, Log, TEXT("[CSTutorialGameMode] Spawned %s (%s) and possessed by %s"), *NewPawn->GetName(), *PawnClass->GetName(), *NewPlayer->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[CSTutorialGameMode] Failed to spawn pawn %s for %s at %s"), *PawnClass->GetName(), *NewPlayer->GetName(), *SpawnLocation.ToString());
	}
}


void ACSTutorialGameMode::RestartPlayer(AController* NewPlayer)
{
	UE_LOG(LogTemp, Log, TEXT("[CSTutorialGameMode] Attempting to restart player %s"), *NewPlayer->GetName());

	UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer); // 마찬가지로 내부에서 선택 정보 로드 시도
	if (!PawnClass || PawnClass == FallbackDefaultPawnClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CSTutorialGameMode] Could not determine PawnClass for respawn for %s. Using fallback."), *NewPlayer->GetName());
		PawnClass = FallbackDefaultPawnClass;
	}

	if (!PawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[CSTutorialGameMode] FallbackDefaultPawnClass is not set! Cannot respawn Pawn for %s."), *NewPlayer->GetName());
		return;
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer);
	FVector SpawnLocation = StartSpot ? StartSpot->GetActorLocation() : FVector::ZeroVector;
	FRotator SpawnRotation = StartSpot ? StartSpot->GetActorRotation() : FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = NewPlayer;
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (NewPawn)
	{
		NewPlayer->Possess(NewPawn);
		UE_LOG(LogTemp, Log, TEXT("[CSTutorialGameMode] Respawned %s (%s) and possessed by %s"), *NewPawn->GetName(), *PawnClass->GetName(), *NewPlayer->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[CSTutorialGameMode] Failed to respawn pawn %s for %s at %s"), *PawnClass->GetName(), *NewPlayer->GetName(), *SpawnLocation.ToString());
	}
}


void ACSTutorialGameMode::HandleMatchIsWaitingToStart()
{
	UE_LOG(LogTemp, Log, TEXT("[CSTutorialGameMode] Match is waiting to start. Transitioning to Playing."));
	HandleStartGame(); // MatchPhase를 EMP_Playing으로 변경
}

UClass* ACSTutorialGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// --- 캐릭터 선택 정보 가져오는 로직 (현재는 주석 처리) ---
	// GameInstance 또는 PlayerState에서 저장된 캐릭터 선택 정보 호출

	// 방법 1: GameInstance 사용 예시 (GameInstance에 변수 정의 필요)
	/*
	UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
	if (GI && GI->SelectedCharacterClass) // GI->SelectedCharacterClass 가 유효하다면
	{
		UE_LOG(LogTemp, Log, TEXT("Retrieved SelectedCharacterClass from GameInstance for %s"), *InController->GetName());
		return GI->SelectedCharacterClass;
	}
	*/

	// 방법 2: PlayerState 사용 예시 (PlayerState에 변수 정의 및 리플리케이션 필요)
	/*
	ACSPlayerState* PS = InController ? InController->GetPlayerState<ACSPlayerState>() : nullptr; // ACSPlayerState로 캐스팅 필요
	if (PS && PS->SelectedCharacterClass) // PS->SelectedCharacterClass 가 유효하다면
	{
		UE_LOG(LogTemp, Log, TEXT("Retrieved SelectedCharacterClass from PlayerState for %s"), *InController->GetName());
		return PS->SelectedCharacterClass;
	}
	*/
	// --- 캐릭터 선택 정보 가져오는 로직 끝 ---


	UE_LOG(LogTemp, Warning, TEXT("[CSTutorialGameMode] Could not retrieve selected character class for %s. Returning fallback pawn class."), *InController->GetName());
	return FallbackDefaultPawnClass; // 정보가 없으면 Fallback 반환
}