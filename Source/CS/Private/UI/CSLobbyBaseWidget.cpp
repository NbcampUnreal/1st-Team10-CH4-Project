#include "UI/CSLobbyBaseWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Controller/CSPlayerController.h"
#include "PlayerStates/CSPlayerState.h"
#include "GameStates/CSLobbyGameState.h"
#include "GameModes/CSLobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/CSPlayerEntry.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "Data/CSCharacterRow.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameStateBase.h"

void UCSLobbyBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ReadyButton) { ReadyButton->OnClicked.AddDynamic(this, &UCSLobbyBaseWidget::OnReadyClicked); }
	if (ExitButton) { ExitButton->OnClicked.AddDynamic(this, &UCSLobbyBaseWidget::OnExitClicked); }

	ReadyCountUpdateInterval = 0.25f;
	TimeSinceLastReadyCountUpdate = ReadyCountUpdateInterval;
}

void UCSLobbyBaseWidget::InitializeLobby(EMatchType CurrentMatchType)
{
	LobbyMatchType = CurrentMatchType;
	bLocalPlayerIsReady = false;
	UpdateLocalReadyStatus(bLocalPlayerIsReady);
	UpdatePlayerList();
}

void UCSLobbyBaseWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	TimeSinceLastReadyCountUpdate += InDeltaTime;
	if (TimeSinceLastReadyCountUpdate >= ReadyCountUpdateInterval)
	{
		TimeSinceLastReadyCountUpdate = 0.0f;

		AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
		if (GS && ReadyCountText)
		{
			int32 CurrentReadyCount = 0;
			int32 TotalPlayerCount = GS->PlayerArray.Num();

			for (APlayerState* Player : GS->PlayerArray)
			{
				ACSPlayerState* CSPlayer = Cast<ACSPlayerState>(Player);
				if (CSPlayer && CSPlayer->bIsReady)
				{
					CurrentReadyCount++;
				}
			}

			FText ReadyCountDisplayText = FText::Format(NSLOCTEXT("LobbyWidget", "ReadyCountFormat", "Ready: {0}/{1}"),
				FText::AsNumber(CurrentReadyCount),
				FText::AsNumber(TotalPlayerCount));
			ReadyCountText->SetText(ReadyCountDisplayText);
		}
	}
}

void UCSLobbyBaseWidget::UpdatePlayerList()
{
	AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
	if (GS) {
		RefreshPlayerList(GS->PlayerArray);
	}
}

void UCSLobbyBaseWidget::RefreshPlayerList(const TArray<APlayerState*>& PlayerArray)
{
}

void UCSLobbyBaseWidget::OnExitClicked()
{
	ACSPlayerController* PC = GetOwningCSPlayerController();
	if (PC)
	{
		PC->Server_RequestReturnToMainMenu();
	}
}


void UCSLobbyBaseWidget::UpdateLocalReadyStatus(bool bIsReady)
{
	bLocalPlayerIsReady = bIsReady;
	if (ReadyButtonText)
	{
		ReadyButtonText->SetText(FText::FromString(bIsReady ? TEXT("CANCEL") : TEXT("READY")));
	}
}

void UCSLobbyBaseWidget::HandleCharacterSelected(EJobTypes SelectedJob)
{
	ACSPlayerController* PC = GetOwningCSPlayerController();
	if (PC) {
		PC->Server_SelectCharacter(SelectedJob);
	}
}

void UCSLobbyBaseWidget::OnReadyClicked()
{
	ACSPlayerController* PC = GetOwningCSPlayerController();
	if (PC) {
		bool bWantsToBeReady = !bLocalPlayerIsReady;
		PC->Server_RequestReady(bWantsToBeReady);
	}
}

void UCSLobbyBaseWidget::SetupCharacterSelection()
{
    if (!CharacterSelectionPanel) return;
    CharacterSelectionPanel->ClearChildren();

    TArray<EJobTypes> AvailableJobs = { EJobTypes::EJT_Fighter, EJobTypes::EJT_SwordMan };

    for (EJobTypes Job : AvailableJobs) {
    }
}