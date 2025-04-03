#include "GameStates/CSLobbyGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Controller/CSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ACSLobbyGameState::OnRep_SelectedMap()
{
    // 맵 바뀔 때 클라에 UI 갱신
    /*if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(PC))
        {
            CSPlayerController->UpdateSelectedMapUI(SelectedMap);
        }
    }*/
}

bool ACSLobbyGameState::AreAllPlayerReady() const
{
    for (APlayerState* PlayerState : PlayerArray)
    {
        if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
        {
            if (!CSPlayerState->bIsReady)
            {
                return false;
            }
        }
    }
    return true;
}

void ACSLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ACSLobbyGameState, SelectedMap);
}
