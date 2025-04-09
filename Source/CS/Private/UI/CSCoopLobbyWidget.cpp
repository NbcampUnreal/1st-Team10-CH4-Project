#include "UI/CSCoopLobbyWidget.h" // 경로 확인
#include "Components/VerticalBox.h"
#include "PlayerStates/CSPlayerState.h"
#include "UI/CSPlayerEntry.h"
#include "Kismet/GameplayStatics.h" // GetGameState
#include "GameStates/CSLobbyGameState.h" // AreAllPlayerReady

void UCSCoopLobbyWidget::InitializeLobby(EMatchType CurrentMatchType)
{
    Super::InitializeLobby(CurrentMatchType); // 부모 초기화 호출
    // Coop 모드 전용 UI 설정 (통합 목록 보이게 등)
    if (Coop_PlayerList) Coop_PlayerList->SetVisibility(ESlateVisibility::Visible);
}

void UCSCoopLobbyWidget::RefreshPlayerList(const TArray<APlayerState*>& PlayerArray)
{
    if (Coop_PlayerList) Coop_PlayerList->ClearChildren(); // 기존 목록 지우기
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
                if (Coop_PlayerList) Coop_PlayerList->AddChildToVerticalBox(EntryWidget);
            }
        }
    }
    // 플레이어 목록 업데이트 후 시작 버튼 활성화 재확인
    APlayerController* PC = GetOwningPlayer();
    if (StartButton && PC && PC->HasAuthority()) {
        AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
        ACSLobbyGameState* LG = Cast<ACSLobbyGameState>(GS);
        bool bAllReady = LG ? LG->AreAllPlayerReady() : false;
        StartButton->SetIsEnabled(GS && GS->PlayerArray.Num() > 0 && bAllReady); // Coop은 1명 이상 가능
    }
}