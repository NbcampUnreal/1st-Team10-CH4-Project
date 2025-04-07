#include "GameModes/CSBossGameMode.h"
#include "GameStates/CSBossGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Ai/Character/AIBossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "EngineUtils.h" // 액터 태그 검색 위해
#include "Save/CSSaveGame.h" // 저장 위해

ACSBossGameMode::ACSBossGameMode()
{
    GameStateClass = ACSBossGameState::StaticClass();
    BossActor = nullptr;
}

void ACSBossGameMode::HandleMatchHasStarted()
{
    Super::HandleMatchHasStarted(); // 플레이어 스폰 등

    // 보스 스폰
    if (!BossCharacterClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[BossGM] 보스 캐릭터 클래스가 설정되지 않았습니다!"));
        return;
    }

    // 보스 스폰 위치 찾기 (태그 사용)
    AActor* SpawnPoint = nullptr;
    for (TActorIterator<AActor> It(GetWorld()); It; ++It)
    {
        if (It->ActorHasTag(BossSpawnTag))
        {
            SpawnPoint = *It;
            break;
        }
    }

    if (SpawnPoint)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        BossActor = GetWorld()->SpawnActor<AAIBossCharacter>(BossCharacterClass, SpawnPoint->GetActorLocation(), SpawnPoint->GetActorRotation(), SpawnParams);

        if (BossActor)
        {
            UE_LOG(LogTemp, Log, TEXT("[BossGM] 보스 스폰됨 at %s"), *SpawnPoint->GetActorLocation().ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[BossGM] 보스 스폰 실패!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[BossGM] 보스 스폰 위치(태그: %s)를 찾을 수 없습니다!"), *BossSpawnTag.ToString());
    }
}


void ACSBossGameMode::InitGameLogic()
{
    Super::InitGameLogic();
    UE_LOG(LogTemp, Log, TEXT("[BossGM] 보스 게임 로직 초기화."));
    // 필요시 보스 게임 상태 초기화
}

ESpawnSlotType ACSBossGameMode::GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState) const
{
    // 싱글 플레이어이므로 간단히 설정
    return ESpawnSlotType::Coop_Player_Slot0;
}

void ACSBossGameMode::HandlePlayerDeath(AController* DeadPlayer)
{
    Super::HandlePlayerDeath(DeadPlayer);
    UE_LOG(LogTemp, Log, TEXT("[BossGM] 플레이어 사망. 보스전 실패."));
    HandleEndGame();
    // 실패 UI 표시
    FTimerHandle ReturnTimer;
    GetWorldTimerManager().SetTimer(ReturnTimer, this, &ACSBossGameMode::ReturnToLobby, 5.0f, false);
}

void ACSBossGameMode::HandleAIDeath(AActor* DeadAI)
{
    Super::HandleAIDeath(DeadAI);
    if (DeadAI == BossActor) // 죽은 AI가 보스인지 확인
    {
        UE_LOG(LogTemp, Log, TEXT("[BossGM] 보스 처치 완료!"));
        HandleVictory();
    }
}

void ACSBossGameMode::HandleVictory()
{
    HandleEndGame(); // 게임 로직 정지

    // --- 캐릭터 해금 및 저장 로직 ---
    UCSSaveGame* SaveGameObject = nullptr;
    const FString SaveSlotName = "PlayerProgress";
    const uint32 UserIndex = 0;

    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
    {
        SaveGameObject = Cast<UCSSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
    }
    else
    {
        SaveGameObject = Cast<UCSSaveGame>(UGameplayStatics::CreateSaveGameObject(UCSSaveGame::StaticClass()));
        // SaveGameObject가 null일 수 있으므로 방어 코드 추가
        if (!SaveGameObject)
        {
            UE_LOG(LogTemp, Error, TEXT("[BossGM] SaveGame 객체 생성 실패!"));
            // 저장 실패 시 처리 (예: 바로 메뉴로 돌아가기)
            FTimerHandle ReturnTimer;
            GetWorldTimerManager().SetTimer(ReturnTimer, this, &ACSBossGameMode::ReturnToLobby, 5.0f, false);
            return;
        }
        SaveGameObject->SaveSlotName = SaveSlotName;
        SaveGameObject->UserIndex = UserIndex;
    }

    if (SaveGameObject)
    {
        // 해금할 캐릭터 ID 추가
        if (!UnlockedCharacterIDOnVictory.IsNone())
        {
            SaveGameObject->UnlockedCharacterIDs.Add(UnlockedCharacterIDOnVictory);
            UE_LOG(LogTemp, Log, TEXT("[BossGM] 캐릭터 %s 해금됨."), *UnlockedCharacterIDOnVictory.ToString());
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("[BossGM] 해금할 캐릭터 ID가 설정되지 않았습니다."));
        }

        // 게임 저장
        if (UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveSlotName, UserIndex))
        {
            UE_LOG(LogTemp, Log, TEXT("[BossGM] 게임 진행 상황 저장 완료."));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[BossGM] 게임 저장 실패!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[BossGM] SaveGame 객체 로드/생성 후에도 Null 상태임!"));
    }
    // --- 저장 로직 끝 ---

    // 승리 UI 표시 (PlayerController 통해)
    // ...

    // 메인 메뉴로 복귀
    FTimerHandle ReturnTimer;
    GetWorldTimerManager().SetTimer(ReturnTimer, this, &ACSBossGameMode::ReturnToLobby, 5.0f, false);
}