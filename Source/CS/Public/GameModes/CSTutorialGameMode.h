#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h" // 새 베이스 클래스 상속
#include "CSTypes/CSGameTypes.h"     // ESpawnSlotType 사용
#include "CSTutorialGameMode.generated.h"

class AController;
class ACSPlayerState; // 전방 선언 추가

/**
 * 튜토리얼 레벨을 위한 게임 모드. (새로운 ACSGameModeBase 상속)
 * 플레이어는 죽지 않으며, 특정 구역 도달 시 다음 스테이지로 이동함.
 * 순수 가상 함수들을 반드시 구현해야 함.
 */
UCLASS()
class CS_API ACSTutorialGameMode : public ACSGameModeBase // 상속 변경 확인
{
	GENERATED_BODY()

public:

	ACSTutorialGameMode();
	// --- ACSGameModeBase의 PURE_VIRTUAL 함수 구현 ---
	/** 튜토리얼 게임 로직 초기화 (모든 플레이어 접속 후 호출됨) */
	virtual void InitGameLogic() override;
	/** 튜토리얼 플레이어 사망 처리 (죽지 않도록 처리) */
	virtual void HandlePlayerDeath(AController* DeadPlayer) override;
	// HandleAIDeath는 필요 시 오버라이드 (현재는 베이스의 빈 함수 사용)

protected:
	// --- ACSGameModeBase의 virtual 함수 오버라이드 ---
	/** 튜토리얼 스폰 슬롯 타입 반환 */
	virtual ESpawnSlotType GetSpawnSlotForPlayer(const ACSPlayerState* PlayerState) const override;
	/** 특정 플레이어에 대해 어떤 Pawn 클래스를 스폰할지 결정 (캐릭터 선택 정보 로드 시도) */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	/** 새로운 플레이어가 게임에 참여할 때 호출됨 (Pawn 스폰 및 무적 처리) */
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	/** 플레이어가 리스타트될 때 호출됨 (무적 처리 포함) */
	virtual void RestartPlayer(AController* NewPlayer) override;
	/** 매치가 시작될 준비가 되었는지 확인 (싱글 플레이어이므로 1명 접속 시 true) */
	virtual bool ReadyToStartMatch_Implementation() override;
	/** 매치가 실제로 시작되었을 때 호출됨 (게임 로직 초기화, 스폰 등) */
	virtual void HandleMatchHasStarted() override;
	/** 플레이어 접속 시 처리 (로그인 수 카운트 및 매치 시작 가능 여부 확인) */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// --- 튜토리얼 자체 기능 ---
public:
	/** 레벨의 트리거가 호출하여 스테이지 이동 시작 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void PlayerEnteredStageTransitionZone(APlayerController* Player);

protected:
	/** 플레이어 Pawn을 무적으로 설정 */
	void MakePlayerInvulnerable(AController* PlayerController);

	/** 캐릭터 선택 정보 없을 때 사용할 기본 Pawn */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	TSubclassOf<APawn> FallbackDefaultPawnClass;

	/** 이동할 다음 스테이지 레벨 이름 */
	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	FName StageLevelName = FName("Stage1Level");

	// InitSinglePlayLogic 함수는 제거됨.
};