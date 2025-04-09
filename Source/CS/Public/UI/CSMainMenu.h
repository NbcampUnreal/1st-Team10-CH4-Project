#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSTypes/CSGameTypes.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TimerManager.h" // 타이머 사용
#include "Kismet/GameplayStatics.h" // 레벨 전환 등
#include "GameInstance/CSGameInstance.h" // 게임 인스턴스 사용
#include "Controller/CSPlayerController.h" // 플레이어 컨트롤러 사용
#include "CSMainMenu.generated.h"

class UButton;
class UTextBlock; // 알림용

UCLASS()
class CS_API UCSMainMenu : public UCSUIBaseWidget
{
	GENERATED_BODY()
public:
	// PlayerController에서 호출하여 알림 메시지 표시
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void ShowNotification(const FText& Message, float Duration = 3.0f);

protected:
	UPROPERTY(meta = (BindWidget)) UButton* ArcadeModeButton;
	UPROPERTY(meta = (BindWidget)) UButton* VersusModeButton;
	UPROPERTY(meta = (BindWidget)) UButton* CoopModeButton;
	UPROPERTY(meta = (BindWidget)) UButton* ExitButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* NotificationText; // 알림 표시용

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override; // 타이머 정리용

private:
	UFUNCTION() void OnArcadeModeClicked();
	UFUNCTION() void OnVersusModeClicked();
	UFUNCTION() void OnCoopModeClicked();
	UFUNCTION() void OnExitClicked();

	// 멀티플레이 모드 시작/참가 요청
	void FindOrCreateMultiplayerSession(EMatchType MatchType);

	// 알림 텍스트 숨기기용 타이머 핸들
	FTimerHandle NotificationTimerHandle;
	UFUNCTION() void HideNotification();
};