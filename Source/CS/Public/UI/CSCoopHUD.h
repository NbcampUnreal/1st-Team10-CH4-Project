#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h"
#include "CSCoopHUD.generated.h"

class UTextBlock;

UCLASS()
class CS_API UCSCoopHUD : public UCSInGameHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Coop")
	void UpdateObjectiveText(const FText& Objective);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Coop")
	void UpdateWaveInfo(int32 CurrentWave, int32 TotalWaves);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Coop")
	void ShowCoopResultMessage(const FText& Message);

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
		TObjectPtr<UTextBlock> ObjectiveDisplayText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
		TObjectPtr<UTextBlock> WaveInfoText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> ResultMessageText;

	// virtual void NativeConstruct() override;
};