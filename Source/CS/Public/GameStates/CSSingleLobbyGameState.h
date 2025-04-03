#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CSSingleLobbyGameState.generated.h"

class APawn; // 전방 선언

/**
 * 싱글 플레이어 로비 레벨의 게임 상태
 * AGameState에서 직접 상속받음
 */
UCLASS()
class CS_API ACSSingleLobbyGameState : public AGameState
{
	GENERATED_BODY()

public:
	ACSSingleLobbyGameState();

	// 로비에 필요한 상태 정보 추가 가능. 예: 선택 가능한 캐릭터 목록

	// --- 선택 가능 캐릭터 목록 (예시, 현재 주석 처리) ---
	/*
	// 클라이언트가 UI를 만드는 데 사용할 수 있도록 리플리케이트 설정.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Lobby")
	TArray<TSubclassOf<APawn>> AvailableCharacterClasses;

	// 리플리케이션 설정
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 서버에서 게임 시작 시 이 함수를 호출하여 목록을 채울 수 있음.
	void LoadAvailableCharacters();
	*/
	// --- 선택 가능 캐릭터 목록 끝 ---

};