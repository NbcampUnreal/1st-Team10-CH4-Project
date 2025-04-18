#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h"
#include "CSBossHUD.generated.h"

// 전방 선언
class UProgressBar;
class UTextBlock;

UCLASS()
class CS_API UCSBossHUD : public UCSInGameHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Boss")
	void UpdateBossHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Boss")
	void UpdateBossName(const FText& BossName);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Boss")
	void ShowBossResultMessage(const FText& Message);

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
		TObjectPtr<UProgressBar> BossHealthBar;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
		TObjectPtr<UTextBlock> BossHealthText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
		TObjectPtr<UTextBlock> BossNameText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> ResultMessageText;

	// virtual void NativeConstruct() override;
};