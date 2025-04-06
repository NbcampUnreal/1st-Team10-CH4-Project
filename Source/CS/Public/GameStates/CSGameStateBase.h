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

	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, BlueprintReadOnly)
	EMatchPhase MatchPhase;

	UPROPERTY(ReplicatedUsing = OnRep_RemainingMatchTime, BlueprintReadOnly)
	int32 RemainingMatchTime;

	UPROPERTY(Replicated)
	EMatchResult MatchResult;

	UFUNCTION()
	void OnRep_MatchPhase();
	UFUNCTION()
	void OnRep_RemainingMatchTime();
	

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
