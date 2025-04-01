#include "GameStates/CSGameStateBase.h"
#include "Net/UnrealNetwork.h"

void ACSGameStateBase::OnRep_MatchPhase()
{
	// Wating, Playing, Finished 게임 상태에 따른 UI 연출
}

void ACSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSGameStateBase, MatchPhase);
}
