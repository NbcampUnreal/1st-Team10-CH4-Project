// CSSingleLobbyGameMode.cpp
#include "GameModes/CSSingleLobbyGameMode.h"
#include "GameStates/CSSingleLobbyGameState.h"
#include "Controller/CSPlayerController.h"
#include "PlayerStates/CSPlayerState.h"
#include "Kismet/GameplayStatics.h"

ACSSingleLobbyGameMode::ACSSingleLobbyGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ACSPlayerController::StaticClass();
	GameStateClass = ACSSingleLobbyGameState::StaticClass();
	PlayerStateClass = ACSPlayerState::StaticClass();
}

void ACSSingleLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer)
	{
		UE_LOG(LogTemp, Log, TEXT("[LobbyGM] 플레이어 %s 로비에 들어옴."), *NewPlayer->GetName());
		if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(NewPlayer))
		{
			CSPlayerController->Client_ShowLobbyUI();
		}
	}
}

void ACSSingleLobbyGameMode::SetPlayerSelection(APlayerController* SelectingPlayer, FName CharacterID)
{
	if (!SelectingPlayer) return;
	ACSPlayerState* PS = SelectingPlayer->GetPlayerState<ACSPlayerState>();
	if (PS)
	{
		UE_LOG(LogTemp, Log, TEXT("[LobbyGM] 플레이어 %s 가 캐릭터 [%s] 선택함."), *SelectingPlayer->GetName(), *CharacterID.ToString());
		PS->SelectedCharacterID = CharacterID;
		// 싱글 플레이어: 선택 즉시 다음 레벨 시작
		StartTutorialLevel(SelectingPlayer);
	}
}

void ACSSingleLobbyGameMode::StartTutorialLevel(APlayerController* Player)
{
	UE_LOG(LogTemp, Log, TEXT("[LobbyGM] 튜토리얼 레벨 %s 로 이동 시작."), *TutorialLevelName.ToString());
	UGameplayStatics::OpenLevel(this, TutorialLevelName);
}

// 메인 메뉴 복귀 함수 추가
void ACSSingleLobbyGameMode::ReturnToMainMenu(APlayerController* Player)
{
	if (!Player) return;

	UE_LOG(LogTemp, Log, TEXT("[LobbyGM] 플레이어 %s 가 메인 메뉴 복귀 요청. 레벨 %s 로 이동."), *Player->GetName(), *MainMenuLevelName.ToString());
	UGameplayStatics::OpenLevel(this, MainMenuLevelName); // 메인 메뉴 레벨 열기
}