#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSTypes/CSGameTypes.h"
#include "CSGameModeBase.generated.h"

/**
 * 전투 전용 GameMode 베이스 클래스.
 * 메뉴/로비 등에서는 사용하지 않음.
 * MatchType에 따라 싱글, 대전, 협동 전투 초기화 로직을 분기 처리함.
 */

UCLASS()
class CS_API ACSGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:
	ACSGameModeBase();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** 매치 상태를 갱신하고 GameState에 전달 */
	void SetMatchPhase(EMatchPhase NewPhase);

protected:
	/** 각 모드별 초기화 로직 */
	virtual void InitGameLogic() PURE_VIRTUAL(ACSGameModeBase::InitGameLogic, );

	/** 게임 시작 처리 (MatchPhase 전환 + 인풋 허용 등) */
	virtual void HandleStartGame();

	/** 게임 종료 처리 (MatchPhase 전환) */
	virtual void HandleEndGame();

	/** 플레이어 사망 처리 (각 MatchType별 정의 필요) */
	virtual void HandlePlayerDeath(AController* DeadPlayer)  PURE_VIRTUAL(ACSGameModeBase::HandlePlayerDeath, );

	/** 모든 플레이어 인풋 허용 설정 */
	void SetAllPlayerInputEnabled(bool bEnabled);

	UPROPERTY(BlueprintReadOnly)
	EMatchType MatchType;

	UPROPERTY(BlueprintReadOnly)
	EMatchPhase MatchPhase;

	UPROPERTY(BlueprintReadOnly)
	int32 LoggedInPlayerCount;

	UPROPERTY(BlueprintReadOnly)
	int32 ExpectedPlayerCount;

	UPROPERTY()
	class UCSGameInstance* CSGameInstance;
	UPROPERTY()
	class ACSGameStateBase* BaseGameState;
};
