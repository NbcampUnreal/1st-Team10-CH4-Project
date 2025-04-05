#include "GameStates/CSVersusGameState.h"
#include "Net/UnrealNetwork.h"

ACSVersusGameState::ACSVersusGameState()
{
	WinningTeamID = -1;
	WinningPlayers.Empty();
}

void ACSVersusGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSVersusGameState, WinningTeamID);
	DOREPLIFETIME(ACSVersusGameState, WinningPlayers);
}
