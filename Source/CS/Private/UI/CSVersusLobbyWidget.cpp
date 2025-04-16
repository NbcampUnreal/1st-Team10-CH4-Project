#include "UI/CSVersusLobbyWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "PlayerStates/CSPlayerState.h"
#include "UI/CSPlayerEntry.h"
#include "Controller/CSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameStates/CSLobbyGameState.h"
#include "GameModes/CSLobbyGameMode.h"

void UCSVersusLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// TObjectPtr 사용 시 -> 또는 Get() 사용, Null 체크 권장
	if (ChangeTeamButton)
	{
		ChangeTeamButton->OnClicked.AddDynamic(this, &UCSVersusLobbyWidget::OnChangeTeamClicked);
	}
}

void UCSVersusLobbyWidget::InitializeLobby(EMatchType CurrentMatchType)
{
	Super::InitializeLobby(CurrentMatchType);
	if (Team0_PlayerList) Team0_PlayerList->SetVisibility(ESlateVisibility::Visible);
	if (Team1_PlayerList) Team1_PlayerList->SetVisibility(ESlateVisibility::Visible);
	if (ChangeTeamButton) ChangeTeamButton->SetVisibility(ESlateVisibility::Visible);
	UE_LOG(LogTemp, Log, TEXT("Versus Lobby Initialized."));
}

void UCSVersusLobbyWidget::RefreshPlayerList(const TArray<APlayerState*>& PlayerArray)
{
	// 목록 비우기 (Null 체크 추가)
	if (Team0_PlayerList) Team0_PlayerList->ClearChildren();
	if (Team1_PlayerList) Team1_PlayerList->ClearChildren();

	if (!PlayerEntryWidgetClass) { UE_LOG(LogTemp, Error, TEXT("PlayerEntryWidgetClass is null!")); return; }

	for (APlayerState* Player : PlayerArray) {
		ACSPlayerState* CSPlayer = Cast<ACSPlayerState>(Player);
		if (CSPlayer) {
			UCSPlayerEntry* EntryWidget = CreateWidget<UCSPlayerEntry>(this, PlayerEntryWidgetClass);
			if (EntryWidget) {
				EntryWidget->SetupPlayerEntry(CSPlayer);
				UVerticalBox* TargetList = (CSPlayer->TeamID == 0) ? Team0_PlayerList.Get() : Team1_PlayerList.Get(); // Get() 사용
				if (TargetList) TargetList->AddChildToVerticalBox(EntryWidget);
			}
		}
	}
}

void UCSVersusLobbyWidget::OnChangeTeamClicked()
{
	ACSPlayerController* PC = GetOwningCSPlayerController();
	if (PC) {
		UE_LOG(LogTemp, Log, TEXT("UI: Change team button clicked."));
		// PlayerController RPC 호출 (주석 해제!)
		PC->Server_RequestTeamChange();
	}
	else { UE_LOG(LogTemp, Warning, TEXT("OnChangeTeamClicked: PlayerController is null!")); }
}