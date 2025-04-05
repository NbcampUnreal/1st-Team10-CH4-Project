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

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	/** 대전 모드 초기화 */
	virtual void InitGameLogic() override;

	/** 게임 시작 시 처리 */
	virtual void HandleStartGame() override;

	/** 플레이어 사망 처리 */
	virtual void HandlePlayerDeath(AController* DeadPlayer) override;

	/** 캐릭터 스폰 및 팀 배치 */
	void SpawnPlayerAtTeamSlots();

	/** 매치 승리 조건 확인 */
	void CheckWinCondition();

	/** 서든데스 발동 */
	void TriggerSuddenDeath();

	/** 매치 종료 처리 */
	void FinishMatch(int32 WinningTeamID);

	/** 로비로 복귀 */
	void ReturnToLobby();

	/** 팀별 생존자 수 갱신 */
	void UpdateAliveTeams(class ACSPlayerState* DeadPlayerState);

	/** 시간 기반 카운트다운 시작 */
	void StartMatchTimeCountDown();

	/** 남은 시간 갱신 */
	void UpdateMatchTime();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameRule")
	int32 MatchTimeLimit;

private:
	class ACSVersusGameState* VersusGameState;

	FTimerHandle MatchTimerHandle;
	FTimerHandle ReturnToLobbyHandle;

	FIntPoint AlivePlayersPerTeam;
};

