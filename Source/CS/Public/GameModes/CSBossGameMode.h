#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h"
#include "CSBossGameMode.generated.h"

class AAIBossCharacter;
class ACSPlayerState;

UCLASS()
class CS_API ACSBossGameMode : public ACSGameModeBase
{
    GENERATED_BODY()

public:
    ACSBossGameMode();

    // --- ACSGameModeBase Overrides ---
    virtual void InitGameLogic() override;
    virtual void HandlePlayerDeath(AController* DeadPlayer) override;
    virtual void HandleAIDeath(AActor* DeadAI) override;

protected:
    virtual ESpawnSlotType GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState) const override;
    virtual void HandleMatchHasStarted() override; // 보스 스폰

    /** 보스 처치 시 호출 */
    void HandleVictory();

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    TSubclassOf<AAIBossCharacter> BossCharacterClass;

    UPROPERTY(EditDefaultsOnly, Category = "AI", meta = (ToolTip = "보스가 스폰될 위치 지정용 액터 태그"))
    FName BossSpawnTag = FName("BossSpawnPoint");

    UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
    FName MainMenuLevelName = FName("MainMenuLevel"); // 승리 후 돌아갈 레벨

    UPROPERTY(EditDefaultsOnly, Category = "Game Flow", meta = (ToolTip = "보스 처치 시 해금될 캐릭터 ID"))
    FName UnlockedCharacterIDOnVictory = FName("NewCharacterID"); // 실제 해금될 캐릭터 ID

    UPROPERTY()
    AActor* BossActor; // 스폰된 보스 액터 참조
};