#pragma once
#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSTypes/CSGameTypes.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Controller/CSPlayerController.h"
#include "PlayerStates/CSPlayerState.h"
#include "GameModes/CSLobbyGameMode.h"
#include "Data/CSCharacterRow.h"
#include "Engine/World.h"
#include "CSLobbyBaseWidget.generated.h"

class UButton;
class UTextBlock;
class UPanelWidget; // 캐릭터 선택 패널용
class UVerticalBox; // 플레이어 목록용
class UCSPlayerEntry; // 플레이어 엔트리 클래스

UCLASS(Abstract) // 이 클래스 자체로 위젯 생성 불가
class CS_API UCSLobbyBaseWidget : public UCSUIBaseWidget
{
    GENERATED_BODY()

public:
    // PlayerController에서 로비 진입 시 호출
    UFUNCTION(BlueprintCallable, Category = "Lobby UI")
    virtual void InitializeLobby(EMatchType CurrentMatchType);

    // PlayerController에서 PlayerArray 변경 시 호출
    UFUNCTION(BlueprintCallable, Category = "Lobby UI")
    virtual void UpdatePlayerList();

    // PlayerController에서 PlayerState 업데이트 시 호출 (BP 구현)
    UFUNCTION(BlueprintImplementableEvent, Category = "Lobby UI")
    void UpdatePlayerEntryUI(class ACSPlayerState* PlayerState);

    // PlayerController에서 로컬 플레이어 Ready 상태 변경 시 호출
    UFUNCTION(BlueprintCallable, Category = "Lobby UI")
    void UpdateLocalReadyStatus(bool bIsReady);

protected:
    // 공통 위젯 바인딩
    UPROPERTY(meta = (BindWidget)) UButton* ReadyButton;
    UPROPERTY(meta = (BindWidget)) UTextBlock* ReadyButtonText;
    UPROPERTY(meta = (BindWidget)) UButton* StartButton; // 호스트만 활성화
    UPROPERTY(meta = (BindWidget)) UPanelWidget* CharacterSelectionPanel; // 캐릭터 선택 버튼들 담을 패널

    // 플레이어 목록용 (파생 클래스에서 구체적인 패널 사용)
    UPROPERTY(EditDefaultsOnly, Category = "Lobby UI")
    TSubclassOf<UCSPlayerEntry> PlayerEntryWidgetClass;

    EMatchType LobbyMatchType = EMatchType::EMT_None;
    bool bLocalPlayerIsReady = false;

    virtual void NativeConstruct() override;

    // 캐릭터 선택 처리 (버튼 클릭 시 호출)
    UFUNCTION()
    void HandleCharacterSelected(EJobTypes SelectedJob);

    UFUNCTION()
    void OnCharacterButtonClicked();

    // 준비 버튼 클릭
    UFUNCTION()
    void OnReadyClicked();

    // 시작 버튼 클릭 (호스트 전용)
    UFUNCTION()
    void OnStartClicked();

    // 플레이어 목록 UI 갱신 (내부 함수)
    virtual void RefreshPlayerList(const TArray<APlayerState*>& PlayerArray);

    // 캐릭터 선택 버튼 생성 및 바인딩 (예시)
    void SetupCharacterSelection();
};
