#include "GameModes/CSStageGameMode.h"
#include "GameStates/CSStageGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Ai/Character/AINormalCharacter.h"
#include "NavigationSystem.h" // 랜덤 위치 스폰 위해
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

ACSStageGameMode::ACSStageGameMode()
{
    // 필요시 GameState, PlayerState 클래스 설정
    GameStateClass = ACSStageGameState::StaticClass();
    // PlayerStateClass = ACSPlayerState::StaticClass(); // 베이스에서 설정되어 있다면 생략 가능
    // DefaultPawnClass = nullptr; // 베이스 스폰 로직 사용

    SpawnedAICount = 0;
    SpawnInterval = 0.f; // BeginPlay 또는 HandleMatchHasStarted 에서 계산
}

void ACSStageGameMode::HandleMatchHasStarted()
{
    Super::HandleMatchHasStarted(); // 플레이어 스폰 등 기본 로직 실행

    // AI 스폰 시작
    if (NumAIToSpawn > 0)
    {
        SpawnInterval = TotalSpawnDuration / FMath::Max(1, NumAIToSpawn); // 0으로 나누기 방지
        GetWorldTimerManager().SetTimer(SpawnAITimerHandle, this, &ACSStageGameMode::SpawnSingleAI, SpawnInterval, true, 0.0f); // 즉시 첫 스폰 시작
        UE_LOG(LogTemp, Log, TEXT("[StageGM] AI 스폰 시작. 총 %d 마리, %f 초 간격."), NumAIToSpawn, SpawnInterval);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageGM] 스폰할 AI 수가 0입니다."));
    }
}


void ACSStageGameMode::InitGameLogic()
{
    Super::InitGameLogic(); // 베이스 로직 호출 (필요 시)

    UE_LOG(LogTemp, Log, TEXT("[StageGM] 스테이지 게임 로직 초기화 (InitGameLogic)."));

    ACSStageGameState* StageGS = GetGameState<ACSStageGameState>();
    if (StageGS)
    {
        StageGS->SetAIInfo(NumAIToSpawn); // 총 AI 수 GameState에 설정
    }

    SpawnedAICount = 0; // 스폰 카운트 초기화
}

ESpawnSlotType ACSStageGameMode::GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState) const
{
    // 싱글 플레이어 스테이지이므로 간단히 첫 번째 슬롯 반환
    // 필요시 PlayerStart 액터를 찾아서 위치 반환 등으로 수정 가능
    return ESpawnSlotType::Coop_Player_Slot0;
}

void ACSStageGameMode::SpawnSingleAI()
{
    if (!AINormalCharacterClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[StageGM] AI Normal Character 클래스가 설정되지 않았습니다!"));
        GetWorldTimerManager().ClearTimer(SpawnAITimerHandle);
        return;
    }

    if (SpawnedAICount >= NumAIToSpawn)
    {
        UE_LOG(LogTemp, Log, TEXT("[StageGM] 모든 AI 스폰 완료 (%d 마리)."), NumAIToSpawn);
        GetWorldTimerManager().ClearTimer(SpawnAITimerHandle);
        return;
    }

    // 랜덤 스폰 위치 찾기 (네비게이션 시스템 활용)
    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
    if (NavSys)
    {
        // 플레이어 주변 또는 특정 영역 내 랜덤 위치 검색
        // 예시: 월드 원점 기준 5000 유닛 반경
        FNavLocation RandomLocation;
        bool bFound = NavSys->GetRandomPointInNavigableRadius(FVector::ZeroVector, 5000.0f, RandomLocation);

        if (bFound)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            AActor* SpawnedAI = GetWorld()->SpawnActor<AAINormalCharacter>(AINormalCharacterClass, RandomLocation.Location, FRotator::ZeroRotator, SpawnParams);

            if (SpawnedAI)
            {
                SpawnedAICount++;
                UE_LOG(LogTemp, Log, TEXT("[StageGM] AI 스폰됨 (%d / %d) at %s"), SpawnedAICount, NumAIToSpawn, *RandomLocation.Location.ToString());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("[StageGM] AI 스폰 실패 at %s"), *RandomLocation.Location.ToString());
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[StageGM] AI 스폰 위한 네비게이션 가능 위치 찾기 실패."));
            // 실패 시 스폰 중단 또는 다른 위치 시도
            GetWorldTimerManager().ClearTimer(SpawnAITimerHandle);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[StageGM] Navigation System 가져오기 실패!"));
        GetWorldTimerManager().ClearTimer(SpawnAITimerHandle);
    }
}


void ACSStageGameMode::HandlePlayerDeath(AController* DeadPlayer)
{
    Super::HandlePlayerDeath(DeadPlayer); // 베이스 호출 (필요 시)

    UE_LOG(LogTemp, Log, TEXT("[StageGM] 플레이어 %s 사망. 스테이지 실패 처리."), *DeadPlayer->GetName());
    HandleEndGame(); // 게임 종료 처리 (입력 차단 등)

    // TODO: 실패 UI 표시 (PlayerController 통해)

    // 일정 시간 후 로비 또는 메인 메뉴로 복귀
    FTimerHandle ReturnTimer;
    GetWorldTimerManager().SetTimer(ReturnTimer, this, &ACSStageGameMode::ReturnToLobby, 5.0f, false);
}

void ACSStageGameMode::HandleAIDeath(AActor* DeadAI)
{
    Super::HandleAIDeath(DeadAI); // 베이스 호출 (필요 시)

    if (Cast<AAINormalCharacter>(DeadAI))
    {
        UE_LOG(LogTemp, Log, TEXT("[StageGM] Normal AI 사망 처리."));
        ACSStageGameState* StageGS = GetGameState<ACSStageGameState>();
        if (StageGS)
        {
            StageGS->DecrementRemainingAICount();
            CheckStageComplete(); // 완료 여부 확인
        }
    }
}

void ACSStageGameMode::CheckStageComplete()
{
    ACSStageGameState* StageGS = GetGameState<ACSStageGameState>();
    if (StageGS && StageGS->GetRemainingAICount() <= 0 && SpawnedAICount >= NumAIToSpawn) // 모든 AI가 스폰되었고, 남은 수가 0 이하일 때
    {
        UE_LOG(LogTemp, Log, TEXT("[StageGM] 모든 AI 처치 완료! 보스 레벨 %s 로 이동."), *BossLevelName.ToString());
        HandleEndGame(); // 게임 로직 정지

        // TODO: 스테이지 클리어 UI 표시

        // 플레이어 상태 유지를 위해 Seamless Travel 사용
        bUseSeamlessTravel = true;
        GetWorld()->ServerTravel(BossLevelName.ToString() + "?listen", true);
    }
}