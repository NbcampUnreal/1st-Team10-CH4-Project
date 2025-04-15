#include "GameModes/CSMainMenuGameMode.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "Controller/CSPlayerController.h"

void ACSMainMenuGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

    if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(NewPlayer))
    {
        const bool bIsListenServer = GetNetMode() == NM_ListenServer;
        CSPlayerController->bIsHostPlayer = bIsListenServer;
    }
}
