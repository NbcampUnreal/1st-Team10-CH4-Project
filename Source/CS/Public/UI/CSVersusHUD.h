// CSVersusHUD.h
#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h"
#include "CSVersusHUD.generated.h"

class UTextBlock;
class ACSVersusGameState; // GameState 사용 위해 전방 선언

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

    // NativeTick 오버라이드 추가
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    // 호스트 점수 업데이트 간격 조절용
    float HostScoreUpdateInterval;
    float TimeSinceLastHostScoreUpdate;

    // GameState 캐싱용
    UPROPERTY()
    TWeakObjectPtr<ACSVersusGameState> CachedVersusGameState;
};