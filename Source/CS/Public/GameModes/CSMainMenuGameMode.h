#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CSTypes/CSGameTypes.h"
#include "CSMainMenuGameMode.generated.h"

UENUM(BlueprintType)
enum class EMainMenuMatchType : uint8
{
	SinglePlayer UMETA(DisplayName = "SinglePlayer"),
	Multiplayer UMETA(DisplayName = "Multiplayer")
};

UCLASS()
class CS_API ACSMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACSMainMenuGameMode();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void StartSinglePlayer();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void StartMultiplayer();

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void QuitGame();

private:
	EMainMenuMatchType MatchType;
};