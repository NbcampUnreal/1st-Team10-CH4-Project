#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h" // 베이스 HUD 상속
#include "CSBossHUD.generated.h"

// 전방 선언
class UProgressBar;
class UTextBlock;

UCLASS()
class CS_API UCSBossHUD : public UCSInGameHUD
{
	GENERATED_BODY()

public:
	// GameState 등에서 호출하여 보스 체력 업데이트 (BP 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Boss")
	void UpdateBossHealth(float CurrentHealth, float MaxHealth);

	// GameState 등에서 호출하여 보스 이름 업데이트 (BP 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Boss")
	void UpdateBossName(const FText& BossName);

	// 추가: 보스 처치/플레이어 사망 시 결과 메시지
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Boss")
	void ShowBossResultMessage(const FText& Message);

protected:
	// UMG 디자이너에서 연결할 위젯들
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) // BlueprintReadOnly 추가
		TObjectPtr<UProgressBar> BossHealthBar;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) // BlueprintReadOnly 추가
		TObjectPtr<UTextBlock> BossHealthText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) // BlueprintReadOnly 추가
		TObjectPtr<UTextBlock> BossNameText;

	// 추가: 결과 메시지용 텍스트
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> ResultMessageText;

	// virtual void NativeConstruct() override;
};