#pragma once
#include "CoreMinimal.h"
#include "UI/CSLobbyBaseWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "PlayerStates/CSPlayerState.h"
#include "UI/CSPlayerEntry.h"
#include "Controller/CSPlayerController.h"
#include "GameStates/CSLobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "CSVersusLobbyWidget.generated.h"

class UVerticalBox;
class UButton;

UCLASS()
class CS_API UCSVersusLobbyWidget : public UCSLobbyBaseWidget
{
    GENERATED_BODY()
protected:
    // 팀별 플레이어 목록
    UPROPERTY(meta = (BindWidget)) UVerticalBox* Team0_PlayerList;
    UPROPERTY(meta = (BindWidget)) UVerticalBox* Team1_PlayerList;
    // 팀 변경 버튼
    UPROPERTY(meta = (BindWidget)) UButton* ChangeTeamButton;

    virtual void NativeConstruct() override;
    // 베이스 함수 오버라이드
    virtual void InitializeLobby(EMatchType CurrentMatchType) override;
    virtual void RefreshPlayerList(const TArray<APlayerState*>& PlayerArray) override;
private:
    UFUNCTION() void OnChangeTeamClicked();
};