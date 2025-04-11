#pragma once

#include "CoreMinimal.h"
#include "UI/CSUIBaseWidget.h"
#include "CSInGameHUD.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS(Abstract)
class CS_API UCSInGameHUD : public UCSUIBaseWidget
{
	GENERATED_BODY()

public:
	// UFUNCTION 제거됨 확인
	virtual void UpdateHealth_Implementation(float CurrentHealth, float MaxHealth) override; // override 키워드 확인

	// UFUNCTION 제거됨 확인
	virtual void UpdateMatchTimer_Implementation(int32 RemainingSeconds) override; // override 키워드 확인

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) // Optional 에도 추가 가능
	TObjectPtr<UTextBlock> HealthText;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UTextBlock> MatchTimerText;

	virtual void NativeConstruct() override;
};