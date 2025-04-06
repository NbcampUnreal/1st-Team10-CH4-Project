#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h"
#include "CSTypes/CSGameTypes.h"
#include "CSCoopGameMode.generated.h"

UCLASS()
class CS_API ACSCoopGameMode : public ACSGameModeBase
{
	GENERATED_BODY()
	
public:
	ACSCoopGameMode();

protected:
	virtual void BeginPlay() override;

	/** 협동 모드 초기화 */
	virtual void InitGameLogic() override;

	/** 플레이어 사망 처리 */
	virtual void HandlePlayerDeath(AController* DeadPlayer) override;

	/** 제한시간 처리 */
	virtual void UpdateMatchTimer() override;

	/** 플레이어별 스폰 슬롯 타입 반환 */
	virtual ESpawnSlotType GetSpawnSlotForPlayer(const class ACSPlayerState* PlayerState) const override;

	/** AI 사망 처리 */
	void HandleAIDeath(AActor* DeadAI) override;

	/** AI 스폰 슬롯 타입 반환 */
	ESpawnSlotType GetSpawnSlotForAI(int32 Index) const;

	/** AI 스폰 처리 */
	void SpawnAIEnemies();

	/** 매치 승리 조건 확인 */
	void CheckWinCondition();

	/** 매치 종료 처리 */
	void FinishMatch(bool bPlayersWin);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class APawn> EnemyAIPawnClass;

	int32 AlivePlayerCount;
	int32 RemainingEnemyCount;

	class ACSCoopGameState* CoopGameState;
};
