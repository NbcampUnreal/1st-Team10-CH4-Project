#include "GameModes/CSMainMenuGameMode.h"
#include "Kismet/GameplayStatics.h"

ACSMainMenuGameMode::ACSMainMenuGameMode()
{
	MatchType = EMainMenuMatchType::SinglePlayer;
}

void ACSMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	// ���� �޴� �ʱ�ȭ ������ ���⿡ �߰��� �� �ֽ��ϴ�.
}

void ACSMainMenuGameMode::StartSinglePlayer()
{
	UGameplayStatics::OpenLevel(this, FName("TutorialMap"));
}

void ACSMainMenuGameMode::StartMultiplayer()
{
	// ��Ƽ�÷��̾� ��带 �����ϴ� ������ ���⿡ �߰��մϴ�.
	// ��: ��Ƽ�÷��̾� �κ�� �̵�
}

void ACSMainMenuGameMode::QuitGame()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("quit");
	}
}