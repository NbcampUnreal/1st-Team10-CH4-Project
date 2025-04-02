#include "GameStates/CSLobbyGameState.h"
#include "PlayerStates/CSPlayerState.h"

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
