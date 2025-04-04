#include "GameModes/CSGameModeBase.h"
#include "GameStates/CSGameStateBase.h"
#include "GameInstance/CSGameInstance.h"
#include "Kismet/GameplayStatics.h"

ACSGameModeBase::ACSGameModeBase()
{
	GameStateClass = ACSGameStateBase::StaticClass();
}

void ACSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (const UCSGameInstance* GI = GetGameInstance<UCSGameInstance>())
	{
		MatchType = GI->MatchType;
	}

	SetMatchPhase(EMatchPhase::EMP_Waiting);
}

void ACSGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ACSGameModeBase::HandleStartGame()
{
	SetMatchPhase(EMatchPhase::EMP_Playing);
}

void ACSGameModeBase::HandleEndGame()
{
	SetMatchPhase(EMatchPhase::EMP_Finished);
}

void ACSGameModeBase::InitMatchLogic()
{
	switch (MatchType)
	{
	case EMatchType::EMT_Single:
		InitSinglePlayLogic();
		break;
	case EMatchType::EMT_Coop:
		InitCoopLogic();
		break;
	case EMatchType::EMT_Versus:
		InitVersusLogic();
		break;
	}
}

void ACSGameModeBase::SetMatchPhase(EMatchPhase NewPhase)
{
	MatchPhase = NewPhase;

	if (ACSGameStateBase* GS = GetGameState<ACSGameStateBase>())
	{
		GS->MatchPhase = NewPhase;
	}
}
