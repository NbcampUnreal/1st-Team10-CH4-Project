#include "PlayerStates/CSPlayerState.h"
#include "GameModes/CSLobbyGameMode.h"
#include "Net/UnrealNetwork.h"

ACSPlayerState::ACSPlayerState()
{
	bReplicates = true;

	bIsReady = false;
	TeamID = -1;
	PlayerIndex = -1;
}

void ACSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 향후 MatchType 기반 초기화 필요 시 GameInstance 참조 가능
	// UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();

}

void ACSPlayerState::SetIsReady(bool bNewReady)
{
	if (HasAuthority())
	{
		bIsReady = bNewReady;

		if (ACSLobbyGameMode* LobbyGameMode = Cast<ACSLobbyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			LobbyGameMode->StartMatchIfReady();
		}
	}
}

void ACSPlayerState::OnRep_IsReady()
{
	// 클라이언트 UI 갱신(Ready 상태)
}

void ACSPlayerState::OnRep_TeamID()
{
	// 클라이언트 UI 갱신(Team 이름 및 색상)
}

void ACSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSPlayerState, bIsReady);
	DOREPLIFETIME(ACSPlayerState, PlayerIndex);
	DOREPLIFETIME(ACSPlayerState, TeamID);
}


