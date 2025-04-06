#pragma once

#include "CoreMinimal.h"
#include "GameStates/CSGameStateBase.h"
#include "CSVersusGameState.generated.h"

UCLASS()
class CS_API ACSVersusGameState : public ACSGameStateBase
{
	GENERATED_BODY()
	
public:
	ACSVersusGameState();

	UPROPERTY(ReplicatedUsing = OnRep_OnSuddenDeath, BlueprintReadOnly)
	bool bIsSuddenDeath;

	UPROPERTY(Replicated)
	int32 WinningTeamID;

	UPROPERTY(Replicated)
	TArray<class ACSPlayerState*> WinningPlayers;

	UFUNCTION()
	void OnRep_OnSuddenDeath();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
