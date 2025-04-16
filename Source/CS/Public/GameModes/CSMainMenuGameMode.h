#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSMainMenuGameMode.generated.h"

UCLASS()
class CS_API ACSMainMenuGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACSMainMenuGameMode() : bHasServerTraveld(false) {}

	virtual void PostLogin(APlayerController* NewPlayer) override;

	bool bHasServerTraveld;
};
