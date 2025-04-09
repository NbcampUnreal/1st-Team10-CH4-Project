#include "GameStates/CSSingleGameState.h"
#include "Net/UnrealNetwork.h"

ACSSingleGameState::ACSSingleGameState()
{
	bIsBossPhase = false;
	DeadAICount = -1;
	TotalAICount = -1;
}

void ACSSingleGameState::OnRep_DeadAICount()
{
	// UI 업데이트
}

void ACSSingleGameState::OnRep_IsBossPhase()
{
	// 보스 단계 UI 업데이트
}

void ACSSingleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSSingleGameState, bIsBossPhase);
	DOREPLIFETIME(ACSSingleGameState, TotalAICount);
	DOREPLIFETIME(ACSSingleGameState, DeadAICount);
}
