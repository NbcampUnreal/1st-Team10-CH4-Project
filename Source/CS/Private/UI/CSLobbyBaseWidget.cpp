#include "UI/CSLobbyBaseWidget.h" // 경로 확인
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "Controller/CSPlayerController.h"
#include "PlayerStates/CSPlayerState.h"
#include "GameStates/CSLobbyGameState.h" // AreAllPlayerReady 사용
#include "GameModes/CSLobbyGameMode.h"   // StartMatchIfReady 사용
#include "Kismet/GameplayStatics.h"
#include "UI/CSPlayerEntry.h"
#include "GameInstance/CSGameInstance.h"
#include "Data/CSCharacterRow.h"
#include "UObject/ConstructorHelpers.h" // StaticEnum

void UCSLobbyBaseWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ReadyButton) // Null 체크 추가
	{
		ReadyButton->OnClicked.AddDynamic(this, &UCSLobbyBaseWidget::OnReadyClicked);
	}
	if (StartButton) // Null 체크 추가
	{
		StartButton->OnClicked.AddDynamic(this, &UCSLobbyBaseWidget::OnStartClicked);
		StartButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	// SetupCharacterSelection(); // BP 에서 처리 권장
}

void UCSLobbyBaseWidget::InitializeLobby(EMatchType CurrentMatchType)
{
	LobbyMatchType = CurrentMatchType;
	bLocalPlayerIsReady = false;
	UpdateLocalReadyStatus(bLocalPlayerIsReady);

	APlayerController* PC = GetOwningPlayer();
	// TObjectPtr 사용 시 -> 또는 Get() 사용, Null 체크 필수
	if (StartButton && PC && PC->HasAuthority())
	{
		StartButton->SetVisibility(ESlateVisibility::Visible);
		StartButton->SetIsEnabled(false); // SetIsEnabled 직접 호출
	}
	UpdatePlayerList();
}

void UCSLobbyBaseWidget::UpdatePlayerList()
{
	AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld()); // 기본 GameState 가져오기
	if (GS) {
		RefreshPlayerList(GS->PlayerArray); // 파생 클래스에서 오버라이드된 함수 호출
	}
	// Base의 UpdatePlayerList 에서는 Start 버튼 로직 제거 (파생 클래스 책임)
}

// 파생 클래스에서 오버라이드 필요
void UCSLobbyBaseWidget::RefreshPlayerList(const TArray<APlayerState*>& PlayerArray)
{
	UE_LOG(LogTemp, Warning, TEXT("UCSLobbyBaseWidget::RefreshPlayerList should be overridden!"));
	// 기본 구현의 Start 버튼 로직 (Fallback)
	APlayerController* PC = GetOwningPlayer();
	if (StartButton && PC && PC->HasAuthority()) {
		// GameState 캐스팅 및 헤더 포함 확인
		ACSLobbyGameState* LG = GetWorld() ? Cast<ACSLobbyGameState>(GetWorld()->GetGameState()) : nullptr;
		bool bAllReady = LG ? LG->AreAllPlayerReady() : false;
		bool bMinPlayerCondition = (PlayerArray.Num() > 0);
		StartButton->SetIsEnabled(bMinPlayerCondition && bAllReady); // SetIsEnabled 직접 호출
	}
}


void UCSLobbyBaseWidget::UpdateLocalReadyStatus(bool bIsReady)
{
	bLocalPlayerIsReady = bIsReady;
	if (ReadyButtonText) // Null 체크
	{
		ReadyButtonText->SetText(FText::FromString(bIsReady ? TEXT("CANCEL") : TEXT("READY")));
	}
}

void UCSLobbyBaseWidget::HandleCharacterSelected(EJobTypes SelectedJob)
{
	ACSPlayerController* PC = GetOwningCSPlayerController();
	if (PC) {
		UE_LOG(LogTemp, Log, TEXT("UI: Character selected: %d. Calling RPC."), (int32)SelectedJob);
		// PC->Server_SelectCharacter(SelectedJob); // 주석 유지
	}
}

void UCSLobbyBaseWidget::OnReadyClicked()
{
	ACSPlayerController* PC = GetOwningCSPlayerController();
	if (PC) {
		bool bWantsToBeReady = !bLocalPlayerIsReady;
		// PC->Server_RequestReady(bWantsToBeReady); // 주석 유지
		UE_LOG(LogTemp, Log, TEXT("UI: Ready button clicked. Requesting ready state: %d"), bWantsToBeReady);
	}
}

void UCSLobbyBaseWidget::OnStartClicked()
{
	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->HasAuthority()) {
		// GameMode 캐스팅 및 헤더 포함 확인
		ACSLobbyGameMode* GM = GetWorld()->GetAuthGameMode<ACSLobbyGameMode>();
		if (GM) {
			UE_LOG(LogTemp, Log, TEXT("UI: Start button clicked by host."));
			GM->StartMatchIfReady();
		}
		else { UE_LOG(LogTemp, Warning, TEXT("UI: Start clicked, but LobbyGameMode not found!")); }
	}
}

void UCSLobbyBaseWidget::SetupCharacterSelection()
{
    if (!CharacterSelectionPanel) return;
    CharacterSelectionPanel->ClearChildren();

    TArray<EJobTypes> AvailableJobs = { EJobTypes::EJT_Fighter, EJobTypes::EJT_SwordMan }; // 예시 직업 목록

    for (EJobTypes Job : AvailableJobs) {
        // BP에서 버튼 미리 만들고 연결하는 것이 더 좋음
        // C++ 동적 생성 예시 (이전 답변 참고, 여기서는 생략하고 BP 방식 권장)
        UE_LOG(LogTemp, Log, TEXT("Character button for %d should be created/handled in Blueprint."), (int32)Job);
    }
}

// 캐릭터 버튼 공통 클릭 핸들러 (BP에서 각 버튼 이벤트를 직접 사용하는 것을 권장)
void UCSLobbyBaseWidget::OnCharacterButtonClicked()
{
    UE_LOG(LogTemp, Warning, TEXT("OnCharacterButtonClicked called. BP implementation recommended!"));
}