#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h"
#include "CSSingleGameMode.generated.h"

UCLASS()
class CS_API ACSSingleGameMode : public ACSGameModeBase
{
	GENERATED_BODY()
	
public:
	ACSSingleGameMode();

	/** 게임 재시작 */
	void RestartSingleGame();

	/** 메인 메뉴로 이동*/
	void ReturnToMainMenu();

protected:
	virtual void BeginPlay() override;

	/** 싱글 모드 초기화 */
	virtual void InitGameLogic() override;

	/** 게임 시작 처리 (AI 로직 시작 포함) */
	virtual void HandleStartGame() override;

	/** 플레이어 사망 처리 */
	virtual void HandlePlayerDeath(AController* DeadPlayer) override;

	/** AI 사망 처리 */
	virtual void HandleAIDeath(AActor* DeadAI) override;

	/** 플레이어 스폰 슬롯 타입 반환 */
	virtual ESpawnSlotType GetSpawnSlotForPlayer(const class ACSPlayerState* PlayerState) const override;

	/** 플레이어 Tag 추가 */
	void InitTagPlayer();

	/** 플레이어 Tag 추가 */
	void InitBossGateActor();

	/** 현재 맵의 AI 수 계산 */
	void CountInitialAI();

	/** 보스 Phase */
	void ActivateBossGate();
	void StartBossPhase();
	void SpawnBoss();

	/** UI용 GameState 상태 갱신 */
	void UpdateGameStateAIStatus();

private:
	UPROPERTY()
	int32 TotalAICount;

	UPROPERTY()
	int32 DeadAICount;

	UPROPERTY()
	TArray<APawn*> PendingAIPawns;

	UPROPERTY(VisibleAnywhere, Category = "Boss")
	class ACSBossGate* BossGateActor;

	UPROPERTY(EditDefaultsOnly, Category = "Boss")
	TSubclassOf<APawn> BossCharacterClass;

	UPROPERTY()
	class ACSSingleGameState* SingleGameState;

};
