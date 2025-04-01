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
	UPROPERTY(ReplicatedUsing = OnRep_MatchPhase, BlueprintReadOnly)
	EMatchPhase MatchPhase;

	UFUNCTION()
	void OnRep_MatchPhase();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
