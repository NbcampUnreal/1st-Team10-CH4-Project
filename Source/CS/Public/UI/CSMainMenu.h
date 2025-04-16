#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSTypes/CSGameTypes.h"
#include "CSMainMenu.generated.h"

class UButton;
class UTextBlock;
class UCSAdvancedGameInstance;

UCLASS()
class CS_API UCSMainMenu : public UCSUIBaseWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void ShowNotification(const FText& Message, float Duration = 3.0f);

	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void EnableMultiplayerButtons();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ArcadeModeButton;

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
	UFUNCTION() void OnExitClicked();

	// 알림 숨기기 타이머 관련
	FTimerHandle NotificationTimerHandle;
	UFUNCTION() void HideNotification();

	// 버튼 비활성화 처리
	void SetMultiplayerButtonsEnabled(bool bEnabled);
};