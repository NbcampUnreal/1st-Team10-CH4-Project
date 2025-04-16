#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSTypes/CSGameTypes.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSLobbyBaseWidget.generated.h"

// 전방 선언
class UButton;
class UTextBlock;
class UPanelWidget;
class UCSPlayerEntry;
class ACSPlayerState;

UCLASS(Abstract)
class CS_API UCSLobbyBaseWidget : public UCSUIBaseWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Lobby UI") virtual void InitializeLobby(EMatchType CurrentMatchType);
	UFUNCTION(BlueprintCallable, Category = "Lobby UI") virtual void UpdatePlayerList();
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby UI") void UpdatePlayerEntryUI(ACSPlayerState* PlayerState);
	UFUNCTION(BlueprintCallable, Category = "Lobby UI") void UpdateLocalReadyStatus(bool bIsReady);

protected:
	// --- 위젯 바인딩 ---
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly) TObjectPtr<UButton> ReadyButton;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly) TObjectPtr<UTextBlock> ReadyButtonText;
	// StartButton 제거됨
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly) TObjectPtr<UPanelWidget> CharacterSelectionPanel;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly) TObjectPtr<UButton> ExitButton; // ExitButton 선언 확인

	// --- 클래스 변수 ---
	UPROPERTY(EditDefaultsOnly, Category = "Lobby UI") TSubclassOf<UCSPlayerEntry> PlayerEntryWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby UI") EMatchType LobbyMatchType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby UI") bool bLocalPlayerIsReady;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
		TObjectPtr<UTextBlock> ReadyCountText;
	// --- 함수 ---
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable, Category = "Lobby Actions") void HandleCharacterSelected(EJobTypes SelectedJob);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION() void OnReadyClicked();
	// OnStartClicked 제거됨
	UFUNCTION() void OnExitClicked();
	virtual void RefreshPlayerList(const TArray<APlayerState*>& PlayerArray);
	UFUNCTION(BlueprintCallable, Category = "Lobby Setup") void SetupCharacterSelection();
	// UFUNCTION() void OnCharacterButtonClicked(); // BP 에서 처리 권장 (유지 또는 제거)

private: // Tick 업데이트 간격 조절용
	float ReadyCountUpdateInterval;
	float TimeSinceLastReadyCountUpdate;
};