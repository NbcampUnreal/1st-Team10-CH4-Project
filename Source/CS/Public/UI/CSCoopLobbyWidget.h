#pragma once
#include "CoreMinimal.h"
#include "UI/CSLobbyBaseWidget.h"
#include "CSCoopLobbyWidget.generated.h"

class UVerticalBox;

UCLASS()
class CS_API UCSCoopLobbyWidget : public UCSLobbyBaseWidget
{
    GENERATED_BODY()
protected:
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UVerticalBox> Coop_PlayerList;
    virtual void InitializeLobby(EMatchType CurrentMatchType) override;
    virtual void RefreshPlayerList(const TArray<APlayerState*>& PlayerArray) override;
};