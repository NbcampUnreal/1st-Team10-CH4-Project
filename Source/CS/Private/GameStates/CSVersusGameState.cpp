#include "GameStates/CSVersusGameState.h"
#include "Controller/CSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACSVersusGameState::ACSVersusGameState()
{
	WinningTeamID = -1;
	WinningPlayers.Empty();
	bIsSuddenDeath = false;
}

void ACSVersusGameState::OnRep_OnSuddenDeath()
{
	if (!bIsSuddenDeath) return;

	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		/*CSPlayerController->OnSuddenDeathUI();*/
	}
}

void ACSVersusGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSVersusGameState, WinningTeamID);
	DOREPLIFETIME(ACSVersusGameState, WinningPlayers);
	DOREPLIFETIME(ACSVersusGameState, bIsSuddenDeath);
}
