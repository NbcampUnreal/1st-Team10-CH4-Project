#pragma once
#include "CoreMinimal.h"
#include "UI/CSLobbyBaseWidget.h"
#include "Components/VerticalBox.h"
#include "PlayerStates/CSPlayerState.h"
#include "UI/CSPlayerEntry.h"
#include "GameStates/CSLobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "CSCoopLobbyWidget.generated.h"

class UVerticalBox;

UCLASS()
class CS_API UCSCoopLobbyWidget : public UCSLobbyBaseWidget
{
    GENERATED_BODY()
protected:
    // 협동 모드 플레이어 목록
    UPROPERTY(meta = (BindWidget)) UVerticalBox* Coop_PlayerList;

    // 베이스 함수 오버라이드
    virtual void InitializeLobby(EMatchType CurrentMatchType) override;
    virtual void RefreshPlayerList(const TArray<APlayerState*>& PlayerArray) override;
};