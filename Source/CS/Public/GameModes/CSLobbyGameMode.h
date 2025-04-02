#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSTypes/CSGameTypes.h"
#include "CSLobbyGameMode.generated.h"

UCLASS()
class CS_API ACSLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ACSLobbyGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void StartMatchIfReady();
	void TryStartMatch();
	void ChangeTeam(class ACSPlayerController* Player);

protected:
	EMatchType MatchType;

	void InitVersusLobby();
	void InitCoopLobby();
	void PositionLobbyCharacters();
	bool IsTeamBalanced();
};
