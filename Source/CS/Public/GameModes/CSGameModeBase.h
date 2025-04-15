#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSTypes/CSGameTypes.h"
#include "CSGameModeBase.generated.h"

/**
 * 전투 전용 GameMode 베이스 클래스.
 * 메뉴/로비 등에서는 사용하지 않음.
 * MatchType에 따른 초기화 로직은 InitGameLogic에서 필수 구현
 * Player 사망 이후 로직은 HandlePlayerDeath에서 필수 구현
 */

UCLASS()
class CS_API ACSGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:
	ACSGameModeBase();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	/** 플레이어 사망 처리 (각 MatchType별 정의 필요) */
	virtual void HandlePlayerDeath(AController* DeadPlayer)  PURE_VIRTUAL(ACSGameModeBase::HandlePlayerDeath, );

	/** AI 사망 처리 (각 MatchType별 정의 필요) */
	virtual void HandleAIDeath(AActor* DeadAI) {}

protected:
	/** 각 모드별 초기화 로직 */
	virtual void InitGameLogic() PURE_VIRTUAL(ACSGameModeBase::InitGameLogic, );

	/** 플레이어별 스폰 슬롯 타입 반환 (게임모드별 오버라이딩 필요) */
	virtual ESpawnSlotType GetSpawnSlotForPlayer(const class ACSPlayerState* PlayerState) { return ESpawnSlotType::None; }

	/** 게임 시작 처리 (MatchPhase 전환 + 인풋 허용) */
	virtual void HandleStartGame();

	/** 게임 종료 처리 (MatchPhase 전환 + 인풋 차단) */
	virtual void HandleEndGame();

	/** 제한 시간 카운트다운 갱신 */
	virtual void UpdateMatchTimer();

	/** 로비로 복귀 */
	virtual void ReturnToLobby();

	/** 플레이어 스폰 처리 */
	void SpawnAllPlayers();

	/** 플레이어 로비 데이터 불러와 저장 */
	void RestorePlayerLobbyData();
	
	/** 월드의 모든 스폰매니저를 타입별로 Map에 저장 */
	TMap<ESpawnSlotType, class ACSSpawnManager*> FindAllSpawnManager() const;

	/** 모든 플레이어 인풋 허용 설정 */
	void SetAllPlayerInputEnabled(bool bEnabled);

	/** AI 전체 StartLogic 처리 */
	void AllAIStartLogic(const TArray<APawn*>& InAIPawns);

	/** 제한 시간 카운트다운 시작 */
	void StartMatchTimer();

	/** 매치 상태를 갱신하고 GameState에 전달 */
	void SetMatchPhase(EMatchPhase NewPhase);

	UPROPERTY(BlueprintReadOnly)
	EMatchType MatchType;
	UPROPERTY(BlueprintReadOnly)
	EMatchPhase MatchPhase;

	UPROPERTY(BlueprintReadOnly)
	int32 LoggedInPlayerCount;
	UPROPERTY(BlueprintReadOnly)
	int32 ExpectedPlayerCount;

	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 MatchTimeLimit;
	
	UPROPERTY()
	class UCSAdvancedGameInstance* CSGameInstance;
	UPROPERTY()
	class ACSGameStateBase* BaseGameState;

	FTimerHandle MatchTimerHandle;
	FTimerHandle ReturnToLobbyHandle;

};
