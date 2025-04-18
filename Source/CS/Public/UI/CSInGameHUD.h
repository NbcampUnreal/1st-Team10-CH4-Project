#pragma once

#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSInGameHUD.generated.h"

class UProgressBar;
class UTextBlock;
class ACSGameStateBase;

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

    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
    float HostTimerUpdateInterval;
    float TimeSinceLastHostTimerUpdate;

    UPROPERTY()
    TWeakObjectPtr<ACSGameStateBase> CachedGameState;
};