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

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION() void OnArcadeModeClicked();
	UFUNCTION() void OnExitClicked();

	FTimerHandle NotificationTimerHandle;
	UFUNCTION() void HideNotification();

	void SetMultiplayerButtonsEnabled(bool bEnabled);
};