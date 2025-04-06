#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h"
#include "CSVersusGameMode.generated.h"

UCLASS()
class CS_API ACSVersusGameMode : public ACSGameModeBase
{
	GENERATED_BODY()

public:
	ACSVersusGameMode();

protected:
	virtual void BeginPlay() override;

	/** 대전 모드 초기화 */
	virtual void InitGameLogic() override;

	/** 플레이어 사망 처리 */
	virtual void HandlePlayerDeath(AController* DeadPlayer) override;

	/** 제한시간 처리 */
	virtual void UpdateMatchTimer() override;

	/** 플레이어별 스폰 슬롯 타입 반환 */
	virtual ESpawnSlotType GetSpawnSlotForPlayer(const class ACSPlayerState* PlayerState) const override;;

	/** 매치 승리 조건 확인 */
	void CheckWinCondition();

	/** 서든데스 발동 */
	void TriggerSuddenDeath();

	/** 매치 종료 처리 */
	void FinishMatch(int32 WinningTeamID);

	/** 팀별 생존자 수 갱신 */
	void UpdateAliveTeams(class ACSPlayerState* DeadPlayerState);

private:
	class ACSVersusGameState* VersusGameState;

	FIntPoint AlivePlayersPerTeam;
};

