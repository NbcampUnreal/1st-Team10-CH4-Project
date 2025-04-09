#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CSTypes/CSGameTypes.h"
#include "CSGameStateBase.generated.h"

UCLASS()
class CS_API ACSGameStateBase : public AGameState
{
	GENERATED_BODY()
	
public:
	ACSGameStateBase();

	UFUNCTION(BlueprintCallable, Category = "Match")
	EMatchPhase GetMatchPhase() const { return MatchPhase; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	void SetMatchPhase(EMatchPhase InPhase) { MatchPhase = InPhase; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	int32 GetRemainingMatchTime() const { return RemainingMatchTime; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	void SetRemainingMatchTime(int32 Time) { RemainingMatchTime = Time; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	EMatchResult GetMatchResult() const { return MatchResult; }

	UFUNCTION(BlueprintCallable, Category = "Match")
	void SetMatchResult(EMatchResult Result) { MatchResult = Result; }

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase)
	EMatchPhase MatchPhase;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingMatchTime)
	int32 RemainingMatchTime;

	UPROPERTY(Replicated)
	EMatchResult MatchResult;

	UFUNCTION()
	void OnRep_MatchPhase();

	UFUNCTION()
	void OnRep_RemainingMatchTime();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
