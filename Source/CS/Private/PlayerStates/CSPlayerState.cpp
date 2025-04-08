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
	SelectedJob = EJobTypes::EJT_None;
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
	SelectedJob = EJobTypes::EJT_None;
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
	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(GetOwner()))
	{
		// UI 갱신 (자기 화면만)
		if (ISLocalPlayerState())
		{
			CSPlayerController->UpdateTeamUI(TeamID);
		}

		// 캐릭터 메시 갱신 (서버 + 모든 클라이언트 모두 변경 필요)
		// RepNotify는 클라의 PlayerState에서만 호출됨.
	}
}

void ACSPlayerState::OnRep_SelectedJob()
{
	// 클라이언트 UI 갱신(Character Mesh 등)
	if (!ISLocalPlayerState()) return;

	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(GetOwner()))
	{
		CSPlayerController->UpdateCharacterUI(SelectedJob);
	}
}

void ACSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACSPlayerState, bIsReady);
	DOREPLIFETIME(ACSPlayerState, PlayerIndex);
	DOREPLIFETIME(ACSPlayerState, TeamID);
	DOREPLIFETIME(ACSPlayerState, SelectedJob);
	DOREPLIFETIME(ACSPlayerState, bIsAlive);
}

bool ACSPlayerState::ISLocalPlayerState() const
{
	APlayerController* LocalController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!LocalController) return false;

	return LocalController->PlayerState == this;
}


