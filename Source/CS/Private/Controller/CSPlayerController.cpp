// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CSPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/CSLobbyGameMode.h"
#include "PlayerStates/CSPlayerState.h"
#include "GameStates/CSLobbyGameState.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSGameStateBase.h"
#include "GameModes/CSSingleLobbyGameMode.h"


ACSPlayerController::ACSPlayerController()
{
}

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (MainMenuWidgetClass)
		{
			MainWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
			if (MainWidgetInstance)
			{
				MainWidgetInstance->AddToViewport();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("MainWidgetInstance is nullptr"));
			}
		}
	}
}

void ACSPlayerController::InitMatchUI()
{
	UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
	if (!GI) return;

	//TSubclassOf<UCSGameUIBase> UIClass = nullptr; // UI 부모클래스

	switch (GI->MatchType)
	{
	case EMatchType::EMT_MainMenu:
		// 예: UIClass = UMainMenuUI::StaticClass();
		break;

	case EMatchType::EMT_Single:
		// 예: UIClass = UCSSingleUI::StaticClass();
		break;

	case EMatchType::EMT_Versus:
		
		break;

	case EMatchType::EMT_Coop:
		
		break;

	default:
		break;
	}

	if (UIClass)
	{
		CurrentUI = CreateWidget<UCSGameUIBase>(this, UIClass);
		CurrentWidget = CurrentUI;

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}

void ACSPlayerController::Client_ShowLobbyUI_Implementation()
{
	if (IsLocalController())
	{
		if (LobbyWidgetInstance != nullptr && LobbyWidgetInstance->IsInViewport()) return;
		LobbyWidgetInstance = CreateWidget<UUserWidget>(this, LobbyWidgetClass);

		if (LobbyWidgetInstance != nullptr)
		{
			LobbyWidgetInstance->AddToViewport();

			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(LobbyWidgetInstance->TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputModeData);
			bShowMouseCursor = true;
		}
	}
}

bool ACSPlayerController::Server_RequestTeamChange_Validate()
{
	// Additional validation can be here if needed

	return true;
}

void ACSPlayerController::Server_RequestTeamChange_Implementation()
{
	ACSLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ACSLobbyGameMode>();

	if (LobbyGameMode)
	{
		LobbyGameMode->ChangeTeam(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Request Team() is not valid"));
	}
}

bool ACSPlayerController::Server_SelectCharacter_Validate(FName CharacterID)
{
	return true;
}

void ACSPlayerController::Server_SelectCharacter_Implementation(FName CharacterID)
{
	ACSLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ACSLobbyGameMode>();

	if (LobbyGameMode)
	{
		LobbyGameMode->SetPlayerSelection(this, CharacterID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectCharacter is not valid"));
	}
}

bool ACSPlayerController::Server_RequestReady_Validate(bool bReady)
{
	return true;
}

void ACSPlayerController::Server_RequestReady_Implementation(bool bReady)
{
	ACSPlayerState* CurrentPlayerState = GetPlayerState<ACSPlayerState>();

	if (CurrentPlayerState)
	{
		CurrentPlayerState->SetIsReady(bReady);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Request Ready() is not valid"));
	}
}

bool ACSPlayerController::Server_SelectMap_Validate(FName Map)
{
	return true;
}

void ACSPlayerController::Server_SelectMap_Implementation(FName Map)
{
	if (HasAuthority())
	{
		ACSLobbyGameState* LobbyGameState = GetWorld()->GetGameState<ACSLobbyGameState>();
		
		if (LobbyGameState)
		{
			LobbyGameState->SelectedMap = Map;
			LobbyGameState->OnRep_SelectedMap();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectMap() is not valid"));
	}
}

bool ACSPlayerController::Server_RequestReturnToMainMenu_Validate()
{
	return true;
}

void ACSPlayerController::Server_RequestReturnToMainMenu_Implementation()
{
	ACSSingleLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ACSSingleLobbyGameMode>();

	if (LobbyGameMode)
	{
		//LobbyGameMode->ReturnToMainMenu(this);
	}
}

void ACSPlayerController::Client_OnSuddenDeath_Implementation()
{
	if (IsLocalController())
	{
		if (MainWidgetInstance)
		{
			// PlayerHUDInstance->OnSuddenDeath();
		}
	}
}

void ACSPlayerController::UpdateReadyUI(bool bReady)
{
	if (IsLocalController() && LobbyWidgetInstance)
	{
		// LobbyWidgetInstance->UpdateReadyUI(bReady);
	}
}

void ACSPlayerController::UpdateTeamUI(int32 TeamID)
{
	if (IsLocalController() && LobbyWidgetInstance)
	{
		// LobbyWidgetInstance->SetTeamState(TeamID);
	}
}

void ACSPlayerController::UpdateCharacterUI(FName SelectedCharacterID)
{
	if (IsLocalController() && LobbyWidgetInstance)
	{
		// LobbyWidgetInstance->SetCharacterState(SelectedCharacterID);
	}
}

void ACSPlayerController::UpdateSelectedMapUI(FName SelectedMap)
{
	if (IsLocalController() && LobbyWidgetInstance)
	{
		// LobbyWidgetInstance->SetMapState(SelectedMap);
	}
}

void ACSPlayerController::UpdateMatchTimeUI(int32 Time)
{
	if (IsLocalController() && MainWidgetInstance)
	{
		// PlayerHUDInstance->UpdateMatchTimeUI(Time);
	}
}

void ACSPlayerController::OnMatchPhaseChanged(EMatchPhase MatchPhase)
{
	/*
	EMatchPhase는 CSTypes / CSGameTypes.h에 정의
	EMP_Playing에 "GameStart!" 메시지 업데이트하거나 EMP_Finished에 전투 승리 UI 업데이트
	싱글, 대전, 협동 매치 타입에 따라 UI가 다를 것이기에 함수 내부에서 GameInstance의 MatchType 에 따른 분기도 나눠야될 것 같습니다.
	GameInstance - MatchType 확인
	GameStateBase - MatchPhase, MatchResult(Versus의 경우 VersusGameState의 WinningTeamID로 확인) 확인
	*/
}

void ACSPlayerController::SetPlayerRole(int PlayerRole)
{
	CharacterRole = PlayerRole;
}

void ACSPlayerController::HealthUpdate(float Health, float MaxHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("Client: %s - HealthUpdate Called. Health: %.1f, MaxHealth: %.1f"), *GetName(), Health, MaxHealth);
	// TODO :HUD validation check
	if (IsLocalController())
	{	
		//TODO : PlayerHUD->UpdateHealth(Health, MaxHealth);
	}
}

