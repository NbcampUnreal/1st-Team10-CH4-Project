#pragma once
#include "CoreMinimal.h"
#include "UI/CSLobbyBaseWidget.h"
#include "Components/Button.h"
#include "CSVersusLobbyWidget.generated.h"

class UVerticalBox;

UCLASS()
class CS_API UCSVersusLobbyWidget : public UCSLobbyBaseWidget
{
    GENERATED_BODY()
protected:
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UVerticalBox> Team0_PlayerList;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UVerticalBox> Team1_PlayerList;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UButton> ChangeTeamButton;

    virtual void NativeConstruct() override;
    virtual void InitializeLobby(EMatchType CurrentMatchType) override;
    virtual void RefreshPlayerList(const TArray<APlayerState*>& PlayerArray) override;
private:
    UFUNCTION() void OnChangeTeamClicked();
};
