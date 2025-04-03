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

	UFUNCTION(BlueprintCallable)
	void StartMatchIfReady();

	UFUNCTION(BlueprintCallable)
	void TryStartMatch();

	UFUNCTION(BlueprintCallable)
	void ChangeTeam(APlayerController* Player);

	UFUNCTION(BlueprintCallable)
	void SetPlayerSelection(APlayerController* Player, FName CharacterID);

protected:
	EMatchType MatchType;

	void PositionLobbyCharacters();
	bool IsTeamBalanced();
	void SetViewLobbyCam(APlayerController* NewPlayer);
};
