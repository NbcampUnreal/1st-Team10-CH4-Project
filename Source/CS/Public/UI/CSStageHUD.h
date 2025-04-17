#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h"
#include "CSStageHUD.generated.h"

class UTextBlock;
class UButton; // ExitButton 위해 추가!

UCLASS()
class CS_API UCSStageHUD : public UCSInGameHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Stage")
	void UpdateRemainingAI(int32 RemainingAI, int32 TotalAI);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Stage")
	void UpdateStageInfo(const FText& StageName);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Stage")
	void TriggerBossAppearSequence();

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> RemainingAIText;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> StageInfoText;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> BossAppearText;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> ResultMessageText;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	TObjectPtr<UButton> ExitButton;

	// virtual void NativeConstruct() override; // 필요 시 오버라이드
};