#include "GameStates/CSSingleGameState.h"
#include "Net/UnrealNetwork.h"

ACSSingleGameState::ACSSingleGameState()
{
	bIsBossPhase = false;
	DeadAICount = -1;
	TotalAICount = -1;
}

void ACSSingleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSSingleGameState, bIsBossPhase);
	DOREPLIFETIME(ACSSingleGameState, TotalAICount);
	DOREPLIFETIME(ACSSingleGameState, DeadAICount);
}
