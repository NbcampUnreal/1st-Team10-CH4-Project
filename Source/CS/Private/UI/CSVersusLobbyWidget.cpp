#include "UI/CSVersusLobbyWidget.h" // 경로 확인
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "PlayerStates/CSPlayerState.h"
#include "UI/CSPlayerEntry.h"
#include "Controller/CSPlayerController.h"
#include "Kismet/GameplayStatics.h" // 추가
#include "GameStates/CSLobbyGameState.h" // 추가
#include "GameModes/CSLobbyGameMode.h"   // IsTeamBalanced 사용 위해 추가

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

	// 시작 버튼 활성화 조건 (호스트만)
	APlayerController* PC = GetOwningPlayer();
	if (StartButton && PC && PC->HasAuthority()) {
		ACSLobbyGameState* LG = GetWorld() ? Cast<ACSLobbyGameState>(GetWorld()->GetGameState()) : nullptr;
		// ACSLobbyGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ACSLobbyGameMode>() : nullptr; // IsTeamBalanced 호출 안 하므로 GM 필요 없을 수 있음

		bool bAllReady = LG ? LG->AreAllPlayerReady() : false;
		// IsTeamBalanced() 호출 제거! GameMode에서 최종 확인하도록 함.
		// bool bTeamBalanced = GM ? GM->IsTeamBalanced() : false;
		bool bPlayerCountOk = PlayerArray.Num() >= 2 && PlayerArray.Num() % 2 == 0;

		// 팀 밸런스 체크 제거하고 플레이어 수와 준비 상태만으로 버튼 활성화 결정
		StartButton->SetIsEnabled(bPlayerCountOk && bAllReady);

		// UE_LOG(LogTemp, Log, TEXT("Versus Start Button Enabled: %s (CountOK: %d, AllReady: %d)"), // 로그에서 Balanced 제거
		//	StartButton->GetIsEnabled() ? TEXT("True") : TEXT("False"), bPlayerCountOk, bAllReady);
	}
}

void UCSVersusLobbyWidget::OnChangeTeamClicked()
{
	ACSPlayerController* PC = GetOwningCSPlayerController();
	if (PC) {
		UE_LOG(LogTemp, Log, TEXT("UI: Change team button clicked."));
		// PC->Server_RequestTeamChange(); // 주석 유지
	}
}