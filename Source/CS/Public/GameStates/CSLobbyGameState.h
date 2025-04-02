#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CSLobbyGameState.generated.h"

UCLASS()
class CS_API ACSLobbyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	bool AreAllPlayerReady() const;

};
