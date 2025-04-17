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

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController && PlayerController->IsLocalController()) 
	{
		ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(PlayerController);
		if (CSPlayerController) 
		{
			UCSUIBaseWidget* CurrentUI = CSPlayerController->GetCurrentUI();
			if (CurrentUI) 
			{
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
