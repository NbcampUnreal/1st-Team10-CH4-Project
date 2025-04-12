// CSInGameHUD.h
#pragma once

#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSInGameHUD.generated.h"

class UProgressBar;
class UTextBlock;
class ACSGameStateBase; // GameState 사용 위해 전방 선언

UCLASS(Abstract)
class CS_API UCSInGameHUD : public UCSUIBaseWidget
{
    GENERATED_BODY()

public:
    virtual void UpdateHealth_Implementation(float CurrentHealth, float MaxHealth) override;
    virtual void UpdateMatchTimer_Implementation(int32 RemainingSeconds) override;

protected:
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UProgressBar> HealthBar;
    UPROPERTY(meta = (BindWidgetOptional)) TObjectPtr<UTextBlock> HealthText;
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> MatchTimerText;

    virtual void NativeConstruct() override;

    // NativeTick 오버라이드 추가!
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    // 호스트 타이머 업데이트 간격 조절용
    float HostTimerUpdateInterval;
    float TimeSinceLastHostTimerUpdate;

    // GameState 캐싱용 (선택적 최적화)
    UPROPERTY()
    TWeakObjectPtr<ACSGameStateBase> CachedGameState;
};