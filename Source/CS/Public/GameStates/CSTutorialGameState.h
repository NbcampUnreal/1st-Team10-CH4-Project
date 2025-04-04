#pragma once

#include "CoreMinimal.h"
#include "GameStates/CSGameStateBase.h" // 부모 클래스 헤더
#include "CSTutorialGameState.generated.h"

/**
 * 튜토리얼 레벨의 게임 상태를 관리함.
 * 현재 목표, 완료 여부 등의 정보를 저장하고 클라이언트에 리플리케이트(복제)할 수 있음.
 */
UCLASS()
class CS_API ACSTutorialGameState : public ACSGameStateBase
{
	GENERATED_BODY()

public:
	ACSTutorialGameState();

	/** 리플리케이트될 프로퍼티를 설정함. */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// --- 튜토리얼 상태 변수 ---

	/** 현재 플레이어에게 표시될 목표 설명 텍스트. 변경 시 OnRep 함수 호출됨. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentObjectiveText, Category = "Tutorial")
	FText CurrentObjectiveText;

	/** 튜토리얼 전체 완료 여부 플래그. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "Tutorial")
	bool bIsTutorialComplete;

	// --- 서버 전용 함수 (GameMode에서 호출) ---

	/**
	 * 서버에서 현재 목표 텍스트를 설정하는 함수.
	 * @param NewText 새로 표시할 목표 텍스트.
	 */
	void SetCurrentObjectiveText(const FText& NewText);

	/** 서버에서 튜토리얼 완료 상태로 설정하는 함수. */
	void CompleteTutorial();

protected:
	/** CurrentObjectiveText 변수가 리플리케이트될 때 클라이언트에서 호출되는 함수. */
	UFUNCTION()
	void OnRep_CurrentObjectiveText();

	// TODO: 필요하다면 튜토리얼 단계 ID (FName), 세부 목표 목록 (TArray<FTutorialObjectiveData> 등) 추가 가능.

};