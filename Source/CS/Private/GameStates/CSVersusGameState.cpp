#include "GameStates/CSVersusGameState.h"
#include "Controller/CSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/CSUIBaseWidget.h"

ACSVersusGameState::ACSVersusGameState()
{
	WinningTeamID = -1;
	WinningPlayers.Empty();
	bIsSuddenDeath = false;
}

void ACSVersusGameState::OnRep_OnSuddenDeath()
{
	if (!bIsSuddenDeath) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC && PC->IsLocalController()) {
		ACSPlayerController* CSPC = Cast<ACSPlayerController>(PC);
		if (CSPC) {
			UCSUIBaseWidget* CurrentUI = CSPC->GetCurrentUI();
			if (CurrentUI) {
				CurrentUI->TriggerSuddenDeathUI();
			}
		}
	}
}

void ACSVersusGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSVersusGameState, WinningTeamID);
	DOREPLIFETIME(ACSVersusGameState, WinningPlayers);
	DOREPLIFETIME(ACSVersusGameState, bIsSuddenDeath);
}
