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

	UPROPERTY(Replicated)
	int32 WinningTeamID;

	UPROPERTY(Replicated)
	TArray<class ACSPlayerState*> WinningPlayers;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
