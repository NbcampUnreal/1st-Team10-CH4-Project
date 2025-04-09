#include "UI/CSVersusLobbyWidget.h" // 경로 확인
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "PlayerStates/CSPlayerState.h"
#include "UI/CSPlayerEntry.h"
#include "Controller/CSPlayerController.h"

void UCSVersusLobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (ChangeTeamButton) ChangeTeamButton->OnClicked.AddDynamic(this, &UCSVersusLobbyWidget::OnChangeTeamClicked);
}

void UCSVersusLobbyWidget::InitializeLobby(EMatchType CurrentMatchType)
{
    Super::InitializeLobby(CurrentMatchType); // 부모 초기화 호출
    // Versus 모드 전용 UI 설정 (팀 목록 보이게 등)
    if (Team0_PlayerList) Team0_PlayerList->SetVisibility(ESlateVisibility::Visible);
    if (Team1_PlayerList) Team1_PlayerList->SetVisibility(ESlateVisibility::Visible);
    if (ChangeTeamButton) ChangeTeamButton->SetVisibility(ESlateVisibility::Visible);
}

void UCSVersusLobbyWidget::RefreshPlayerList(const TArray<APlayerState*>& PlayerArray)
{
    // 기존 목록 지우기
    if (Team0_PlayerList) Team0_PlayerList->ClearChildren();
    if (Team1_PlayerList) Team1_PlayerList->ClearChildren();

    if (!PlayerEntryWidgetClass) return;

    for (APlayerState* Player : PlayerArray)
    {
        ACSPlayerState* CSPlayer = Cast<ACSPlayerState>(Player);
        if (CSPlayer)
        {
            UCSPlayerEntry* EntryWidget = CreateWidget<UCSPlayerEntry>(this, PlayerEntryWidgetClass);
            if (EntryWidget)
            {
                EntryWidget->SetupPlayerEntry(CSPlayer);
                // 팀 ID에 따라 적절한 목록에 추가
                UVerticalBox* TargetList = (CSPlayer->TeamID == 0) ? Team0_PlayerList : Team1_PlayerList;
                if (TargetList) TargetList->AddChildToVerticalBox(EntryWidget);
            }
        }
    }
    // 플레이어 목록 업데이트 후 시작 버튼 활성화 재확인
    APlayerController* PC = GetOwningPlayer();
    if (StartButton && PC && PC->HasAuthority()) {
        AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
        ACSLobbyGameState* LG = Cast<ACSLobbyGameState>(GS);
        bool bAllReady = LG ? LG->AreAllPlayerReady() : false;
        StartButton->SetIsEnabled(GS && GS->PlayerArray.Num() >= 2 && bAllReady); // Versus는 최소 2명 필요
    }
}

void UCSVersusLobbyWidget::OnChangeTeamClicked()
{
    ACSPlayerController* PC = GetOwningCSPlayerController();
    if (PC)
    {
        UE_LOG(LogTemp, Log, TEXT("Change team button clicked."));
        // PC->Server_RequestTeamChange(); // PlayerController RPC 호출
        // 임시 주석 처리
    }
}