#include "PlayerStates/CSPlayerState.h"
#include "GameModes/CSLobbyGameMode.h"
#include "Controller/CSPlayerController.h"
#include "Characters/CSBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACSPlayerState::ACSPlayerState()
{
	bReplicates = true;

	bIsReady = false;
	bIsAlive = true;
	TeamID = -1;
	PlayerIndex = -1;
	SelectedCharacterID = NAME_None;
}

void ACSPlayerState::BeginPlay()
{
	Super::BeginPlay();

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

void ACSPlayerState::ResetLobbySettings()
{
	bIsReady = false;
	SelectedCharacterID = NAME_None;
	bIsAlive = true;
}

void ACSPlayerState::OnRep_IsReady()
{
	// 클라이언트 UI 갱신(Ready 상태)
	if (!ISLocalPlayerState()) return;
	
	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(GetOwner()))
	{
		/*CSPlayerController->UpdateReadyUI(bIsReady);*/
	}
}

void ACSPlayerState::OnRep_TeamID()
{
	// 클라이언트 UI 갱신(Team 이름 및 색상)
	if (!ISLocalPlayerState()) return;
	
	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(GetOwner()))
	{
		/*CSPlayerController->UpdateTeamUI(TeamID);*/
	}
}

void ACSPlayerState::OnRep_CharacterID()
{
	// 클라이언트 UI 갱신(Character Mesh 등)
	if (!ISLocalPlayerState()) return;

	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(GetOwner()))
	{
		/*CSPlayerController->UpdateCharacterUI(SelectedCharacterID);*/
	}
}

void ACSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSPlayerState, bIsReady);
	DOREPLIFETIME(ACSPlayerState, PlayerIndex);
	DOREPLIFETIME(ACSPlayerState, TeamID);
	DOREPLIFETIME(ACSPlayerState, SelectedCharacterID);
	DOREPLIFETIME(ACSPlayerState, bIsAlive);
}

bool ACSPlayerState::ISLocalPlayerState() const
{
	APlayerController* LocalController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!LocalController) return false;

	return LocalController->PlayerState == this;
}


