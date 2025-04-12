#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h" // 베이스 HUD 상속
#include "CSCoopHUD.generated.h"

// 전방 선언
class UTextBlock;

UCLASS()
class CS_API UCSCoopHUD : public UCSInGameHUD
{
	GENERATED_BODY()

public:
	// CoopGameState 등에서 호출하여 목표 텍스트 업데이트 (BP 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Coop")
	void UpdateObjectiveText(const FText& Objective);

	// CoopGameState 등에서 호출하여 웨이브 정보 업데이트 (BP 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Coop")
	void UpdateWaveInfo(int32 CurrentWave, int32 TotalWaves);

	// 추가: 협동 모드 결과 메시지 (예: "Mission Complete", "Mission Failed")
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Coop")
	void ShowCoopResultMessage(const FText& Message);

protected:
	// UMG 디자이너에서 연결
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) // BlueprintReadOnly 추가
		TObjectPtr<UTextBlock> ObjectiveDisplayText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) // BlueprintReadOnly 추가
		TObjectPtr<UTextBlock> WaveInfoText;

	// 추가: 결과 메시지용 텍스트
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> ResultMessageText;

	// virtual void NativeConstruct() override;
};