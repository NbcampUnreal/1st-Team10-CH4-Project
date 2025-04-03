#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSSingleLobbyGameMode.generated.h"

class ACSSingleLobbyGameState; // 전방 선언

/**
 * 싱글 플레이어 로비 및 캐릭터 선택 레벨을 위한 게임 모드.
 * AGameMode에서 직접 상속받음.
 */
UCLASS()
class CS_API ACSSingleLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACSSingleLobbyGameMode();

	/** 플레이어가 로그인(로비 레벨 진입)할 때 호출됨. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/**
	 * 캐릭터 선택 완료 후 게임 시작(튜토리얼 레벨 이동)을 요청할 때 호출됨.
	 * 보통 PlayerController가 서버 RPC로 이 함수를 호출
	 * @param SelectingPlayer 게임 시작을 요청한 플레이어 컨트롤러.
	 * @param SelectedCharacterClass 플레이어가 선택한 캐릭터 클래스 (이 정보 전달 방식은 확정 필요).
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void RequestStartTutorial(APlayerController* SelectingPlayer /*, TSubclassOf<APawn> SelectedCharacterClass*/); // 캐릭터 정보 받는 부분 추가 필요

protected:
	/** 캐릭터 선택 후 이동할 튜토리얼 레벨 이름. */
	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	FName TutorialLevelName = FName("TutorialLevel"); // 기본값

private:
	// 현재 로비에 있는 플레이어 수 등을 추적할 필요가 있다면 변수 추가 가능.
};