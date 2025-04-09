#include "UI/CSLobbyBaseWidget.h" // 경로 확인
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Components/VerticalBox.h"
#include "Controller/CSPlayerController.h"
#include "PlayerStates/CSPlayerState.h"
#include "GameStates/CSLobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/CSPlayerEntry.h"
#include "GameInstance/CSGameInstance.h" // 캐릭터 정보 가져오기 위해
#include "Data/CSCharacterRow.h" // 캐릭터 정보 구조체
#include "UObject/ConstructorHelpers.h" // StaticEnum 사용 시 필요할 수 있음

void UCSLobbyBaseWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (ReadyButton) ReadyButton->OnClicked.AddDynamic(this, &UCSLobbyBaseWidget::OnReadyClicked);
    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &UCSLobbyBaseWidget::OnStartClicked);
        StartButton->SetVisibility(ESlateVisibility::Collapsed); // 기본적으로 숨김 (호스트만 표시)
    }

    // 캐릭터 선택 UI 초기화
    SetupCharacterSelection();
}

void UCSLobbyBaseWidget::InitializeLobby(EMatchType CurrentMatchType)
{
    LobbyMatchType = CurrentMatchType;
    UpdatePlayerList(); // 초기 플레이어 목록 표시
    UpdateLocalReadyStatus(bLocalPlayerIsReady); // 레디 버튼 초기화

    // 호스트인지 확인하고 시작 버튼 표시/숨김
    APlayerController* PC = GetOwningPlayer();
    if (StartButton && PC && PC->HasAuthority()) // 리슨 서버 호스트는 Authority 가짐
    {
        StartButton->SetVisibility(ESlateVisibility::Visible);
        StartButton->SetIsEnabled(false); // 초기엔 비활성화 (모두 Ready 상태일 때 활성화)
    }
}

void UCSLobbyBaseWidget::UpdatePlayerList()
{
    AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
    if (GS)
    {
        RefreshPlayerList(GS->PlayerArray);
    }
    // 시작 버튼 활성화 조건 확인 (호스트만)
    APlayerController* PC = GetOwningPlayer();
    if (StartButton && PC && PC->HasAuthority())
    {
        ACSLobbyGameState* LG = Cast<ACSLobbyGameState>(GS); // GameState 캐스팅
        bool bAllReady = LG ? LG->AreAllPlayerReady() : false; // GameState 함수 사용
        // 플레이어가 1명 이상이고 모두 준비되었을 때 활성화
        StartButton->SetIsEnabled(GS && GS->PlayerArray.Num() > 0 && bAllReady);
    }
}

// 플레이어 목록 갱신 (파생 클래스에서 구현 필요)
void UCSLobbyBaseWidget::RefreshPlayerList(const TArray<APlayerState*>& PlayerArray)
{
    // 이 함수는 파생 클래스(Versus, Coop)에서 구체적인 리스트(팀별, 통합)에
    // PlayerEntry 위젯을 추가/제거하도록 오버라이드되어야 합니다.
    UE_LOG(LogTemp, Warning, TEXT("UCSLobbyBaseWidget::RefreshPlayerList should be overridden in derived classes!"));
}

void UCSLobbyBaseWidget::UpdateLocalReadyStatus(bool bIsReady)
{
    bLocalPlayerIsReady = bIsReady;
    if (ReadyButtonText) ReadyButtonText->SetText(FText::FromString(bIsReady ? TEXT("CANCEL") : TEXT("READY")));
    // TODO: 버튼 스타일 변경
}

void UCSLobbyBaseWidget::HandleCharacterSelected(EJobTypes SelectedJob)
{
    ACSPlayerController* PC = GetOwningCSPlayerController();
    if (PC) {
        UE_LOG(LogTemp, Log, TEXT("Character selected in UI: %d"), (int32)SelectedJob);
        PC->Server_SelectCharacter(SelectedJob); // RPC 호출 확인
    }
}

void UCSLobbyBaseWidget::OnReadyClicked()
{
    ACSPlayerController* PC = GetOwningCSPlayerController();
    if (PC)
    {
        bool bWantsToBeReady = !bLocalPlayerIsReady;
        // PC->Server_RequestReady(bWantsToBeReady); // PlayerController RPC 호출
        UE_LOG(LogTemp, Log, TEXT("Ready button clicked. Requesting ready state: %d"), bWantsToBeReady);
        // 임시 주석 처리
    }
    // UpdateLocalReadyStatus(bWantsToBeReady); // 로컬 UI 즉시 반영 (서버 응답 전에)
}

void UCSLobbyBaseWidget::OnStartClicked()
{
    APlayerController* PC = GetOwningPlayer();
    // 호스트만 시작 가능
    if (PC && PC->HasAuthority())
    {
        ACSLobbyGameMode* GM = GetWorld()->GetAuthGameMode<ACSLobbyGameMode>();
        if (GM)
        {
            UE_LOG(LogTemp, Log, TEXT("Start button clicked by host. Starting match..."));
            GM->StartMatchIfReady(); // 게임 시작 시도
        }
    }
}

void UCSLobbyBaseWidget::SetupCharacterSelection()
{
    if (!CharacterSelectionPanel) return;
    CharacterSelectionPanel->ClearChildren();

    TArray<EJobTypes> AvailableJobs = { EJobTypes::EJT_Fighter, EJobTypes::EJT_SwordMan };

    for (EJobTypes Job : AvailableJobs)
    {
        UButton* CharButton = NewObject<UButton>(this);
        if (CharButton)
        {
            // 버튼 내용 설정 (이전과 동일)
            UTextBlock* ButtonText = NewObject<UTextBlock>(CharButton);
            if (ButtonText) { /* ... 텍스트 설정 ... */ CharButton->AddChild(ButtonText); }

            // CharButton->SetFName(...); // 이 줄 삭제!

           // 클릭 이벤트에 공통 핸들러 연결 (AddDynamic 사용)
            CharButton->OnClicked.AddDynamic(this, &UCSLobbyBaseWidget::OnCharacterButtonClicked);

            CharacterSelectionPanel->AddChild(CharButton);
        }
    }
}

// 캐릭터 버튼 공통 클릭 핸들러 구현 (BP 사용 권장 메시지 유지)
void UCSLobbyBaseWidget::OnCharacterButtonClicked()
{
    // !!! 중요: 이 방식은 어떤 버튼이 클릭되었는지 직접 알 수 없음 !!!
    // UMG 블루프린트에서 각 버튼에 직접 OnClicked 이벤트를 만들고,
    // 해당 이벤트 그래프에서 HandleCharacterSelected(EJobTypes::SpecificJob) 를 호출하는 것이 좋습니다.
    UE_LOG(LogTemp, Warning, TEXT("OnCharacterButtonClicked called. Which button? BP implementation recommended!"));
    // HandleCharacterSelected(EJobTypes::EJT_Fighter); // 임시 처리 제거 또는 주석 처리
}
