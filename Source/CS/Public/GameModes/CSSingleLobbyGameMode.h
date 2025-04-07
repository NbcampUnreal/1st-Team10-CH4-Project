// CSSingleLobbyGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSSingleLobbyGameMode.generated.h"

class ACSSingleLobbyGameState;
class ACSPlayerController;
class APlayerController;

UCLASS()
class CS_API ACSSingleLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACSSingleLobbyGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// PlayerController의 Server_SelectCharacter RPC에서 호출됨
	void SetPlayerSelection(APlayerController* SelectingPlayer, FName CharacterID);

	// PlayerController의 새 Server RPC (예: Server_RequestReturnToMainMenu)에서 호출됨
	void ReturnToMainMenu(APlayerController* Player);

protected:
	// SetPlayerSelection 내부에서 호출되어 레벨 이동 실행
	void StartTutorialLevel(APlayerController* Player);

	// 이동할 레벨 이름 설정
	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	FName TutorialLevelName = FName("TutorialLevel");

	UPROPERTY(EditDefaultsOnly, Category = "Game Flow")
	FName MainMenuLevelName = FName("MainMenuLevel"); // 메인 메뉴 레벨 이름 추가
};