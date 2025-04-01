#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSTypes/CSGameTypes.h"
#include "CSGameModeBase.generated.h"

UCLASS()
class CS_API ACSGameModeBase : public AGameMode
{
	GENERATED_BODY()
	
public:
	ACSGameModeBase();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void HandleStartGame();
	virtual void HandleEndGame();

protected:
	UPROPERTY(BlueprintReadOnly)
	EMatchType MatchType;

	UPROPERTY(BlueprintReadOnly)
	EMatchPhase MatchPhase;

	virtual void InitMatchLogic();

	virtual void InitSinglePlayLogic() {}
	virtual void InitVersusLogic() {}
	virtual void InitCoopLogic() {}

	void SetMatchPhase(EMatchPhase NewPhase);
	
};
