#include "UI/CSMainMenu.h" // 경로 확인
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "Controller/CSPlayerController.h"
#include "CSTypes/CSGameTypes.h"
#include "TimerManager.h" // 알림 타이머
#include "Engine/World.h" // GetWorld()
#include "Engine/NetDriver.h" // GetNetMode()

void UCSMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	// 버튼 클릭 이벤트 바인딩
	if (ArcadeModeButton) ArcadeModeButton->OnClicked.AddDynamic(this, &UCSMainMenu::OnArcadeModeClicked);
	if (ExitButton) ExitButton->OnClicked.AddDynamic(this, &UCSMainMenu::OnExitClicked);

	if (NotificationText) NotificationText->SetVisibility(ESlateVisibility::Collapsed); // 처음엔 숨김

	UE_LOG(LogTemp, Warning, TEXT("✅ NativeConstruct CALLED on: %s"), *GetName());
}

void UCSMainMenu::NativeDestruct()
{
	// 위젯 소멸 시 타이머 확실히 정리
	if (UWorld* World = GetWorld()) // GetWorld 유효성 검사 추가
	{
		World->GetTimerManager().ClearTimer(NotificationTimerHandle);
	}
	Super::NativeDestruct();
}

void UCSMainMenu::OnArcadeModeClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Arcade Mode Clicked"));
	UCSAdvancedGameInstance* GI = GetGameInstance<UCSAdvancedGameInstance>();
	UWorld* World = GetWorld(); // GetWorld() 호출
	if (GI && World) {
		GI->SetMatchType(EMatchType::EMT_Single);
		RemoveFromParent(); // 위젯 제거
		// GI->StartArcadeMode(); // GameInstance 함수 사용
		UGameplayStatics::OpenLevel(World, FName("SingleModeLevel")); // 직접 레벨 열기 (레벨 이름 확인)
		UE_LOG(LogTemp, Log, TEXT("Opening SingleModeLevel for Arcade Mode."));
	}
	else { UE_LOG(LogTemp, Error, TEXT("Failed to get GameInstance or World for Arcade Mode.")); }
}

void UCSMainMenu::ShowNotification(const FText& Message, float Duration)
{
	if (NotificationText)
	{
		NotificationText->SetText(Message);
		NotificationText->SetVisibility(ESlateVisibility::HitTestInvisible);

		UWorld* World = GetWorld();
		if (!World) return; // World 유효성 검사

		World->GetTimerManager().ClearTimer(NotificationTimerHandle);
		if (Duration > 0.0f) {
			World->GetTimerManager().SetTimer(NotificationTimerHandle, this, &UCSMainMenu::HideNotification, Duration, false);
			// 에러 메시지 표시 후 버튼 재활성화 (예: "세션 없음" 알림 시)
			EnableMultiplayerButtons(); // 버튼 재활성화 함수 호출
		}
		// Duration 0.0f (예: "검색 중...") 일 때는 HideNotification이나 EnableMultiplayerButtons 호출 안 함
		// -> 세션 결과 콜백에서 처리 필요
	}
}

void UCSMainMenu::HideNotification()
{
	if (NotificationText) {
		NotificationText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

// 멀티플레이 관련 버튼 활성화/비활성화 함수
void UCSMainMenu::SetMultiplayerButtonsEnabled(bool bEnabled)
{
	if (VersusModeButton) VersusModeButton->SetIsEnabled(bEnabled);
	if (CoopModeButton) CoopModeButton->SetIsEnabled(bEnabled);
	// 아케이드나 종료 버튼은 항상 활성화되도록 유지 (필요시 수정)
}

// GameInstance 콜백에서 실패 시 버튼 재활성화를 위해 호출될 수 있음
void UCSMainMenu::EnableMultiplayerButtons()
{
	SetMultiplayerButtonsEnabled(true);
}


void UCSMainMenu::OnExitClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Exit Clicked"));
	// GetOwningPlayer() 대신 GameplayStatics 사용
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC) PC->ConsoleCommand("quit");
}

// 제거된 FindOrCreateMultiplayerSession 함수
// void UCSMainMenu::FindOrCreateMultiplayerSession(EMatchType MatchType) { ... }