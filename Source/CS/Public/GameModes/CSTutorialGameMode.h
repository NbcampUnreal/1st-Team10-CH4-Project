#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h"
#include "CSTutorialGameMode.generated.h"

class APawn;
class AController;
class APlayerController;

/**
 * 튜토리얼 레벨을 위한 게임 모드.
 * 플레이어는 이 모드에서 죽지 않으며, 특정 구역 도달 시 다음 스테이지로 이동함.
 */
UCLASS()
class CS_API ACSTutorialGameMode : public ACSGameModeBase
{
	GENERATED_BODY()

public:
	ACSTutorialGameMode();

	virtual void HandleMatchIsWaitingToStart() override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void PlayerEnteredStageTransitionZone(APlayerController* Player);

protected:
	virtual void InitGameLogic() override; // GameModeBase 리펙토링으로 인한 함수 이름 수정

	// PostLogin은 특별히 변경할 내용 없으면 부모 클래스 것 사용.

	/** 캐릭터 선택 정보를 가져오지 못했을 경우 사용할 기본 Pawn 클래스. */
	UPROPERTY(EditDefaultsOnly, Category = "Defaults")
	TSubclassOf<APawn> FallbackDefaultPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	FName StageLevelName = FName("Stage1Level");

private:
	/** 플레이어 Pawn을 무적으로 설정하는 내부 함수. */
	void MakePlayerInvulnerable(AController* PlayerController);
};