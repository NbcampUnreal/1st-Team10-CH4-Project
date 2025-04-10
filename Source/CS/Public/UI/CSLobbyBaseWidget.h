#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSTypes/CSGameTypes.h"
#include "CSTypes/CSCharacterTypes.h" // EJobTypes
#include "Components/Button.h"
#include "CSLobbyBaseWidget.generated.h"

// 전방 선언
class UButton;
class UTextBlock;
class UPanelWidget;
class UVerticalBox; // 예시: 플레이어 목록용
class UCSPlayerEntry;

UCLASS(Abstract) // 직접 위젯 생성 불가
class CS_API UCSLobbyBaseWidget : public UCSUIBaseWidget
{
	GENERATED_BODY()

public:
	// PlayerController에서 로비 진입 시 호출 (MatchType 전달)
	UFUNCTION(BlueprintCallable, Category = "Lobby UI")
	virtual void InitializeLobby(EMatchType CurrentMatchType);

	// PlayerController 또는 PlayerState 변경 시 호출하여 전체 플레이어 목록 UI 갱신
	UFUNCTION(BlueprintCallable, Category = "Lobby UI")
	virtual void UpdatePlayerList();

	// PlayerState의 OnRep 함수 등에서 호출되어 특정 플레이어 항목 UI 업데이트 (BP 구현)
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby UI")
	void UpdatePlayerEntryUI(class ACSPlayerState* PlayerState);

	// 로컬 플레이어 Ready 상태 변경 시 UI 업데이트
	UFUNCTION(BlueprintCallable, Category = "Lobby UI")
	void UpdateLocalReadyStatus(bool bIsReady);

protected:
	// --- 위젯 바인딩 ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ReadyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ReadyButtonText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartButton; // 호스트만 활성화/사용

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> CharacterSelectionPanel; // 캐릭터 선택 버튼들 담을 패널

	// --- 클래스 변수 ---
	UPROPERTY(EditDefaultsOnly, Category = "Lobby UI")
	TSubclassOf<UCSPlayerEntry> PlayerEntryWidgetClass; // 플레이어 항목 위젯 BP 클래스

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby UI")
	EMatchType LobbyMatchType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lobby UI")
	bool bLocalPlayerIsReady;

	// --- 함수 ---
	virtual void NativeConstruct() override;

	// 캐릭터 선택 버튼 클릭 처리 (BP 또는 C++ 파생 클래스에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Lobby Actions")
	void HandleCharacterSelected(EJobTypes SelectedJob);

	// 준비 버튼 클릭 핸들러
	UFUNCTION()
	void OnReadyClicked();

	// 시작 버튼 클릭 핸들러 (호스트 전용)
	UFUNCTION()
	void OnStartClicked();

	// 플레이어 목록 UI 갱신 (파생 클래스에서 오버라이드 필요)
	virtual void RefreshPlayerList(const TArray<APlayerState*>& PlayerArray);

	// 캐릭터 선택 UI 영역 초기화 (BP 또는 C++에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Lobby Setup")
	void SetupCharacterSelection();

	// 캐릭터 버튼 클릭 공통 핸들러 (AddDynamic 연결용)
	UFUNCTION()
	void OnCharacterButtonClicked(); // 구현은 BP 권장
};