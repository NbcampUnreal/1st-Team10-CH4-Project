#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h"
#include "CSStageGameMode.generated.h"

class AAINormalCharacter;
class ACSPlayerState;

UCLASS()
class CS_API ACSStageGameMode : public ACSGameModeBase
{
    GENERATED_BODY()

public:
    ACSStageGameMode();

    // --- ACSGameModeBase Overrides ---
    virtual void InitGameLogic() override;
    virtual void HandlePlayerDeath(AController* DeadPlayer) override;
    virtual void HandleAIDeath(AActor* DeadAI) override; // AI 사망 처리

protected:
    virtual ESpawnSlotType GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState) const override;
    virtual void HandleMatchHasStarted() override; // 플레이어 스폰 후 AI 스폰 시작

    // --- Stage Specific Logic ---
    /** 스테이지 완료 조건 확인 및 다음 레벨 이동 */
    void CheckStageComplete();

    /** 설정된 간격으로 AI를 하나씩 스폰 */
    void SpawnSingleAI();

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TSubclassOf<AAINormalCharacter> AINormalCharacterClass;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    int32 NumAIToSpawn = 30;

    UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ToolTip = "총 AI 스폰에 걸리는 시간(초)"))
    float TotalSpawnDuration = 10.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
    FName BossLevelName = FName("BossLevel"); // 실제 보스 레벨 이름

    FTimerHandle SpawnAITimerHandle;
    int32 SpawnedAICount;
    float SpawnInterval; // 스폰 간격 (계산됨)
};