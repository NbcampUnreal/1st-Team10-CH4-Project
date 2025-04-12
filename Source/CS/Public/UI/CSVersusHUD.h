#pragma once

#include "CoreMinimal.h"
#include "UI/CSInGameHUD.h" // 베이스 HUD 상속
#include "CSVersusHUD.generated.h"

// 전방 선언
class UTextBlock;

UCLASS()
class CS_API UCSVersusHUD : public UCSInGameHUD
{
	GENERATED_BODY()

public:
	// VersusGameState 등에서 호출하여 팀 점수 업데이트 (BP 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Versus")
	void UpdateTeamScores(int32 Team0Score, int32 Team1Score);

	// 라운드 정보 업데이트 등 추가 가능
	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Versus")
	// void UpdateRoundInfo(int32 CurrentRound, int32 TotalRounds);

	// 추가: VS 모드 전용 메시지 표시 (예: "Team 0 Wins!")
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD | Versus")
	void ShowVersusResultMessage(const FText& Message);

protected:
	// UMG 디자이너에서 연결
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) // BlueprintReadOnly 추가
		TObjectPtr<UTextBlock> Team0ScoreText;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly) // BlueprintReadOnly 추가
		TObjectPtr<UTextBlock> Team1ScoreText;

	// UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	// TObjectPtr<UTextBlock> RoundInfoText;

	// 추가: 결과 메시지용 텍스트
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UTextBlock> ResultMessageText;

	// virtual void NativeConstruct() override; // 필요 시 오버라이드
};