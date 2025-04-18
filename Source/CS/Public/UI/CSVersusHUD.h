#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h"
#include "CSVersusHUD.generated.h"

class UTextBlock;
class ACSVersusGameState;

UCLASS()
class CS_API UCSVersusHUD : public UCSInGameHUD
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Versus")
    void UpdateTeamScores(int32 Team0Score, int32 Team1Score);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Versus")
    void ShowVersusResultMessage(const FText& Message);

protected:
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) TObjectPtr<UTextBlock> Team0ScoreText;
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) TObjectPtr<UTextBlock> Team1ScoreText;
    UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) TObjectPtr<UTextBlock> ResultMessageText;

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    float HostScoreUpdateInterval;
    float TimeSinceLastHostScoreUpdate;

    UPROPERTY()
    TWeakObjectPtr<ACSVersusGameState> CachedVersusGameState;
};