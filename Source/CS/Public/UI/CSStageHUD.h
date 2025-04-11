#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h" // 베이스 HUD 상속
#include "CSStageHUD.generated.h"

// 전방 선언
class UTextBlock;

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
	void TriggerBossAppearSequence(); // 이전 추가된 함수;

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> RemainingAIText;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> StageInfoText;
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> BossAppearText; // 추가 예시

	// virtual void NativeConstruct() override; // 필요 시 오버라이드
};