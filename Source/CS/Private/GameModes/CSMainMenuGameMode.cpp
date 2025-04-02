#include "GameModes/CSMainMenuGameMode.h"
#include "Kismet/GameplayStatics.h"

ACSMainMenuGameMode::ACSMainMenuGameMode()
{
	MatchType = EMainMenuMatchType::SinglePlayer;
}

void ACSMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	// 메인 메뉴 초기화 로직을 여기에 추가할 수 있습니다.
}

void ACSMainMenuGameMode::StartSinglePlayer()
{
	UGameplayStatics::OpenLevel(this, FName("TutorialMap"));
}

void ACSMainMenuGameMode::StartMultiplayer()
{
	// 멀티플레이어 모드를 시작하는 로직을 여기에 추가합니다.
	// 예: 멀티플레이어 로비로 이동
}

void ACSMainMenuGameMode::QuitGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("quit");
	}
}