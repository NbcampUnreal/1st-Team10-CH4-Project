#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSTypes/CSGameTypes.h"
// #include "Components/Button.h" // .h 에서는 전방 선언으로 대체 가능
// #include "Components/TextBlock.h"
// #include "TimerManager.h"
// #include "Kismet/GameplayStatics.h"
// #include "GameInstance/CSGameInstance.h"
// #include "Controller/CSPlayerController.h"
// 헤더 파일 포함 최소화 위해 전방 선언 사용, *테스트 후 주석 삭제 예정*
#include "CSMainMenu.generated.h"

class UButton;
class UTextBlock;
class UCSGameInstance; // 전방 선언 추가

UCLASS()
class CS_API UCSMainMenu : public UCSUIBaseWidget
{
	GENERATED_BODY()
public:
	// PlayerController에서 호출하여 알림 메시지 표시
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void ShowNotification(const FText& Message, float Duration = 3.0f);

	// GameInstance 콜백 등에서 버튼 재활성화 위해 호출 (선택적)
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void EnableMultiplayerButtons();

protected:
	// --- 위젯 바인딩 ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ArcadeModeButton; // TObjectPtr 사용 권장

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> VersusModeButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CoopModeButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NotificationText;

	// --- 생명 주기 ---
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	// --- 버튼 클릭 핸들러 ---
	UFUNCTION() void OnArcadeModeClicked();
	UFUNCTION() void OnVersusModeClicked();
	UFUNCTION() void OnCoopModeClicked();
	UFUNCTION() void OnExitClicked();

	// --- 내부 로직 ---
	// 멀티플레이 버튼 클릭 시 공통 처리 (NetMode 확인 후 GI 호출)
	void HandleMultiplayerButtonClick(EMatchType MatchType);

	// 알림 숨기기 타이머 관련
	FTimerHandle NotificationTimerHandle;
	UFUNCTION() void HideNotification();

	// 버튼 비활성화 처리
	void SetMultiplayerButtonsEnabled(bool bEnabled);
};