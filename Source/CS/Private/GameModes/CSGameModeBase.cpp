#include "GameModes/CSGameModeBase.h"
#include "GameStates/CSGameStateBase.h"
#include "GameInstance/CSGameInstance.h"
#include "PlayerStates/CSPlayerState.h"
#include "Kismet/GameplayStatics.h"

ACSGameModeBase::ACSGameModeBase()
{
	CSGameInstance = nullptr;
	BaseGameState = nullptr;

	MatchType = EMatchType::EMT_None;
	MatchPhase = EMatchPhase::EMP_None;

	LoggedInPlayerCount = 0;
	ExpectedPlayerCount = 0;
}

void ACSGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CSGameInstance = GetGameInstance<UCSGameInstance>();
	BaseGameState = GetGameState<ACSGameStateBase>();

	if (CSGameInstance)
	{
		MatchType = CSGameInstance->MatchType;
		ExpectedPlayerCount = CSGameInstance->ExpectedPlayerCount;
	}
}

void ACSGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	LoggedInPlayerCount++;

	if (LoggedInPlayerCount == ExpectedPlayerCount)
	{
		InitGameLogic();
	}
}

void ACSGameModeBase::HandleStartGame()
{
	SetMatchPhase(EMatchPhase::EMP_Playing);
	SetAllPlayerInputEnabled(true);
}

void ACSGameModeBase::HandleEndGame()
{
	SetMatchPhase(EMatchPhase::EMP_Finished);
	SetAllPlayerInputEnabled(false);
}

void ACSGameModeBase::SetAllPlayerInputEnabled(bool bEnabled)
{
	for (APlayerState* PlayerState : BaseGameState->PlayerArray)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(PlayerState->GetOwner()))
		{
			if (APawn* Pawn = PlayerController->GetPawn())
			{
				bEnabled ? Pawn->EnableInput(PlayerController) : Pawn->DisableInput(PlayerController);
			}
		}
	}
}

void ACSGameModeBase::SetMatchPhase(EMatchPhase NewPhase)
{
	MatchPhase = NewPhase;

	if (BaseGameState)
	{
		BaseGameState->MatchPhase = NewPhase;
	}
}
