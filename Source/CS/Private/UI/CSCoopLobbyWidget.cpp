#include "UI/CSCoopLobbyWidget.h" // 경로 확인
#include "Components/VerticalBox.h"
#include "PlayerStates/CSPlayerState.h"
#include "UI/CSPlayerEntry.h"
#include "Kismet/GameplayStatics.h"
#include "GameStates/CSLobbyGameState.h"
#include "Controller/CSPlayerController.h" // 추가 (GetOwningPlayer)

void UCSCoopLobbyWidget::InitializeLobby(EMatchType CurrentMatchType)
{
	Super::InitializeLobby(CurrentMatchType);
	if (Coop_PlayerList) Coop_PlayerList->SetVisibility(ESlateVisibility::Visible); // Null 체크
	UE_LOG(LogTemp, Log, TEXT("Coop Lobby Initialized."));
}

void UCSCoopLobbyWidget::RefreshPlayerList(const TArray<APlayerState*>& PlayerArray)
{
	if (Coop_PlayerList) Coop_PlayerList->ClearChildren(); // Null 체크
	if (!PlayerEntryWidgetClass) { UE_LOG(LogTemp, Error, TEXT("PlayerEntryWidgetClass is null!")); return; }

	for (APlayerState* Player : PlayerArray) {
		ACSPlayerState* CSPlayer = Cast<ACSPlayerState>(Player);
		if (CSPlayer) {
			UCSPlayerEntry* EntryWidget = CreateWidget<UCSPlayerEntry>(this, PlayerEntryWidgetClass);
			if (EntryWidget) {
				EntryWidget->SetupPlayerEntry(CSPlayer);
				if (Coop_PlayerList) Coop_PlayerList->AddChildToVerticalBox(EntryWidget); // Null 체크
			}
		}
	}

	// 시작 버튼 활성화 조건 (호스트만)
	APlayerController* PC = GetOwningPlayer();
	if (StartButton && PC && PC->HasAuthority()) { // Null 체크
		// GameState 캐스팅 및 헤더 포함 확인
		ACSLobbyGameState* LG = GetWorld() ? Cast<ACSLobbyGameState>(GetWorld()->GetGameState()) : nullptr;
		bool bAllReady = LG ? LG->AreAllPlayerReady() : false;
		bool bPlayerCountOk = PlayerArray.Num() >= 1 && PlayerArray.Num() <= 4;

		StartButton->SetIsEnabled(bPlayerCountOk && bAllReady); // -> 사용

		// GetIsEnabled() 호출 시에도 -> 또는 Get() 사용
		// UE_LOG(LogTemp, Log, TEXT("Coop Start Button Enabled: %s (CountOK: %d, AllReady: %d)"),
		// 	StartButton->GetIsEnabled() ? TEXT("True") : TEXT("False"), bPlayerCountOk, bAllReady);
	}
}