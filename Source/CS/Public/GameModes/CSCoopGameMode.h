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

	/** 게임 시작 처리 (MatchPhase 전환 + 인풋 허용) */
	virtual void HandleStartGame() override;

	/** 플레이어 사망 처리 */
	virtual void HandlePlayerDeath(AController* DeadPlayer) override;

	/** 제한시간 처리 */
	virtual void UpdateMatchTimer() override;

	/** 플레이어별 스폰 슬롯 타입 반환 */
	virtual ESpawnSlotType GetSpawnSlotForPlayer(const class ACSPlayerState* PlayerState) const override;

	/** AI 전체 StartLogic 처리 */
	void AllAIStartLogic();

	/** AI 사망 처리 */
	void HandleAIDeath(AActor* DeadAI) override;

	/** AI 스폰 슬롯 타입 반환 */
	ESpawnSlotType GetSpawnSlotForAI(int32 Index) const;

	/** AI 스폰 처리 */
	void SpawnAIEnemies();

	/** 랜덤 AI Row 반환 */
	TSubclassOf<APawn> SelectRandomAIClass();

	/** 매치 승리 조건 확인 */
	void CheckWinCondition();

	/** 매치 종료 처리 */
	void FinishMatch(bool bPlayersWin);

private:
	UPROPERTY()
	TArray<APawn*> PendingAIPawns;

	int32 AlivePlayerCount;
	int32 RemainingEnemyCount;

	class ACSCoopGameState* CoopGameState;
};
