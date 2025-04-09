#include "PlayerStates/CSPlayerState.h"
#include "GameModes/CSLobbyGameMode.h"
#include "Controller/CSPlayerController.h"
#include "Characters/CSBaseCharacter.h"
#include "UI/CSUIBaseWidget.h" // 위젯 헤더 추가
#include "UI/CSLobbyBaseWidget.h" // 로비 위젯 헤더 추가
#include "UI/CSPlayerEntry.h" // 플레이어 엔트리 헤더 추가
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
    // 로컬 플레이어이고, PlayerController 와 현재 UI 위젯이 유효할 때만
    if (ISLocalPlayerState())
    {
        ACSPlayerController* PC = GetOwner<ACSPlayerController>();
        if (PC)
        {
            UCSUIBaseWidget* CurrentUI = PC->GetCurrentUI(); // PlayerController의 GetCurrentUI() 호출
            if (CurrentUI)
            {
                // 현재 UI에서 직접 함수 호출
                CurrentUI->UpdateReadyStatus(bIsReady);

                // 만약 로비라면, 로비 위젯의 특정 함수 호출 시도
                UCSLobbyBaseWidget* LobbyWidget = Cast<UCSLobbyBaseWidget>(CurrentUI);
                if (LobbyWidget)
                {
                    LobbyWidget->UpdateLocalReadyStatus(bIsReady); // 로컬 플레이어 상태 업데이트 함수 호출
                    // 자신의 엔트리 업데이트 (선택적)
                    // LobbyWidget->UpdatePlayerEntryUI(this);
                }
            }
        }
    }
    // 모든 클라이언트에서 다른 플레이어의 상태 변경 시 UI 업데이트
    ACSPlayerController* PC = GetOwner<ACSPlayerController>();
    if (PC && !ISLocalPlayerState()) // 다른 플레이어의 변경 알림
    {
        UCSUIBaseWidget* CurrentUI = PC->GetCurrentUI();
        UCSLobbyBaseWidget* LobbyWidget = Cast<UCSLobbyBaseWidget>(CurrentUI);
        if (LobbyWidget)
        {
            LobbyWidget->UpdatePlayerEntryUI(this); // 해당 플레이어 엔트리 업데이트 요청
        }
    }
}

void ACSPlayerState::OnRep_TeamID()
{
    // 팀 변경은 주로 로비에서 발생
    ACSPlayerController* PC = GetOwner<ACSPlayerController>();
    if (PC)
    {
        UCSUIBaseWidget* CurrentUI = PC->GetCurrentUI();
        UCSLobbyBaseWidget* LobbyWidget = Cast<UCSLobbyBaseWidget>(CurrentUI);

        if (LobbyWidget)
        {
            // 로컬 플레이어의 팀 변경 시 특별한 UI 업데이트 (필요 시)
            if (ISLocalPlayerState())
            {
                LobbyWidget->UpdateTeamDisplay(TeamID); // 위젯의 팀 표시 업데이트
            }
            // 모든 플레이어 목록 갱신 (팀 이동 반영 위해)
            LobbyWidget->UpdatePlayerList();
        }
        // 캐릭터 메시 변경 등 다른 로직은 여기에 추가
    }
}

void ACSPlayerState::OnRep_SelectedJob()
{
    ACSPlayerController* PC = GetOwner<ACSPlayerController>();
    if (PC)
    {
        UCSUIBaseWidget* CurrentUI = PC->GetCurrentUI();
        if (CurrentUI)
        {
            // 로컬 플레이어의 직업 변경 시 UI 업데이트
            if (ISLocalPlayerState())
            {
                CurrentUI->UpdateCharacterDisplay(SelectedJob); // 위젯의 캐릭터 표시 업데이트
            }

            // 로비 위젯이라면 해당 플레이어 엔트리 업데이트
            UCSLobbyBaseWidget* LobbyWidget = Cast<UCSLobbyBaseWidget>(CurrentUI);
            if (LobbyWidget)
            {
                LobbyWidget->UpdatePlayerEntryUI(this);
            }
        }
        // 캐릭터 외형 변경 등 다른 로직 추가
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


