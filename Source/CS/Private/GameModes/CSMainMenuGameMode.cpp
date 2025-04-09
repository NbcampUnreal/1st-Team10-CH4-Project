#include "GameModes/CSMainMenuGameMode.h"
#include "GameInstance/CSGameInstance.h"
#include "Controller/CSPlayerController.h"

void ACSMainMenuGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ACSPlayerController* PlayerController = Cast<ACSPlayerController>(NewPlayer))
	{
		// 메인메뉴 UI 뷰포트에 추가(자동으로 메인메뉴 UI 보여지면 불필요)
	}
}