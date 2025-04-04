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

	virtual void HandleStartGame();
	virtual void HandleEndGame();
	virtual void HandlePlayerDeath(AController* DeadPlayer) {}

protected:
	UPROPERTY(BlueprintReadOnly)
	EMatchType MatchType;

	UPROPERTY(BlueprintReadOnly)
	EMatchPhase MatchPhase;

	virtual void InitMatchLogic();

	virtual void InitSinglePlayLogic() {}
	virtual void InitVersusLogic() {}
	virtual void InitCoopLogic() {}

	void SetMatchPhase(EMatchPhase NewPhase);
	
};
