#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h"
#include "CSTutorialGameMode.generated.h"

/**
 * 튜토리얼 레벨을 위한 게임 모드
 * 실제 전투/게임플레이 로직이 시작
 */
UCLASS()
class CS_API ACSTutorialGameMode : public ACSGameModeBase
{
	GENERATED_BODY()

public:
	ACSTutorialGameMode();

	virtual void HandleMatchIsWaitingToStart() override;

	/** 특정 플레이어에 대해 어떤 Pawn 클래스를 스폰할지 결정합니다. */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	// 튜토리얼 목표 달성, 종료 등 관련 함수 추가 가능
	// UFUNCTION(...)
	// void CompleteTutorialObjective(APlayerController* Player, FName ObjectiveID);

protected:
	virtual void InitSinglePlayLogic() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	TSubclassOf<APawn> FallbackDefaultPawnClass;
};