#pragma once

#include "CoreMinimal.h"
#include "GameModes/CSGameModeBase.h"
#include "CSVersusGameMode.generated.h"

UCLASS()
class CS_API ACSVersusGameMode : public ACSGameModeBase
{
	GENERATED_BODY()

public:
	ACSVersusGameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void InitVersusLogic() override;

	void SpawnPlayerAtTeamSlots();
	void StartMatchCountDown();
	void HandlePlayerDeath(AController* DeadPlayer);
	void CheckWinCondition();
	void TriggerSuddenDeath();
	void FinishMatch(int32 WinningTeamID);
	void ReturnToLobby();

private:
	FTimerHandle ReturnToLobbyHandle;

	bool bIsSuddenDeath;
	float MatchTimeLimit;
	float ElapsedTime;

	int32 AliveTeam0;
	int32 AliveTeam1;

	void UpdateAliveTeams();
};

