#pragma once

#include "CoreMinimal.h"
#include "GameStates/CSGameStateBase.h"
#include "PlayerStates/CSPlayerState.h"
#include "CSVersusGameState.generated.h"

UCLASS()
class CS_API ACSVersusGameState : public ACSGameStateBase
{
	GENERATED_BODY()
	
public:
	ACSVersusGameState();

	UPROPERTY(ReplicatedUsing = OnRep_OnSuddenDeath, BlueprintReadOnly)
	bool bIsSuddenDeath;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Versus State")
	int32 WinningTeamID;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Versus State")
	TArray<TObjectPtr<ACSPlayerState>> WinningPlayers;

	UFUNCTION()
	void OnRep_OnSuddenDeath();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
