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
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void InitVersusLogic() override;
	virtual void HandleStartGame() override;
	virtual void HandlePlayerDeath(AController* DeadPlayer) override;

	void SpawnPlayerAtTeamSlots();
	void CheckWinCondition();
	void TriggerSuddenDeath();
	void FinishMatch(int32 WinningTeamID);
	void ReturnToLobby();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameRule")
	int32 MatchTimeLimit;

private:

	class UCSGameInstance* CSGameInstance;
	class ACSVersusGameState* CSGameState;

	FTimerHandle MatchTimerHandle;
	FTimerHandle ReturnToLobbyHandle;

	FIntPoint AlivePlayersPerTeam;

	int32 LoggedInPlayerCount;
	int32 ExpectedPlayerCount;

	void UpdateAliveTeams(class ACSPlayerState* PlayerState);
	void StartMatchTimeCountDown();
	void UpdateMatchTime();
};

