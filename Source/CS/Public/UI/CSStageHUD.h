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
	// GameState 등에서 호출하여 남은 적 정보 업데이트 (BP 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Stage")
	void UpdateRemainingAI(int32 RemainingAI, int32 TotalAI); // 총 AI 수도 받도록 수정

	// GameState 등에서 호출하여 현재 스테이지 정보 업데이트 (BP 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Stage")
	void UpdateStageInfo(const FText& StageName);

protected:
	// UMG 디자이너에서 연결할 위젯들
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RemainingAIText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StageInfoText;

	// virtual void NativeConstruct() override; // 필요 시 오버라이드
};