#include "GameModes/CSMainMenuGameMode.h"
#include "GameInstance/CSGameInstance.h"
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

bool ACSMainMenuGameMode::CheackShouldStartTravel()
{
    if (UCSGameInstance* CSGameInstance = GetGameInstance<UCSGameInstance>())
    {
        return CSGameInstance->bIsSessionCreated;
    }

    return false;
}
