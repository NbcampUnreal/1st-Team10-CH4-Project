#pragma once

#include "CoreMinimal.h"
#include "GameStates/CSGameStateBase.h"
#include "CSSingleGameState.generated.h"

UCLASS()
class CS_API ACSSingleGameState : public ACSGameStateBase
{
	GENERATED_BODY()
	
public:
	ACSSingleGameState();

	UFUNCTION(BlueprintCallable, Category = "AI")
	int32 GetTotalAICount() const { return TotalAICount; }

	UFUNCTION(BlueprintCallable, Category = "AI")
	int32 GetDeadAICount() const { return DeadAICount; }

	void SetTotalAICount(int32 InTotal) { TotalAICount = InTotal; }
	void SetDeadAICount(int32 InDead) { DeadAICount = InDead; }
	void SetBossPhase(bool bBossPhase) { bIsBossPhase = bBossPhase; }

protected:
	UFUNCTION()
	void OnRep_DeadAICount();

	UFUNCTION()
	void OnRep_IsBossPhase();

	UPROPERTY(ReplicatedUsing = OnRep_IsBossPhase)
	bool bIsBossPhase;

	UPROPERTY(ReplicatedUsing = OnRep_DeadAICount)
	int32 DeadAICount;

	UPROPERTY(Replicated)
	int32 TotalAICount;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
