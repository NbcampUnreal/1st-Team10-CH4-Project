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
#include "UI/CSUIBaseWidget.h"


ACSPlayerController::ACSPlayerController()
{
}


void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		//// Temporary code to show the main menu UI
		// 
		//if (MainMenuWidgetClass)
		//{
		//	MainWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		//	if (MainWidgetInstance)
		//	{
		//		MainWidgetInstance->AddToViewport();
		//	}
		//	else
		//	{
		//		UE_LOG(LogTemp, Warning, TEXT("MainWidgetInstance is nullptr"));
		//	}
		//}

		InitMatchUI(); // PostLogin 에서 호출 예정
	}
}

void ACSPlayerController::InitMatchUI()
{
	if (CurrentActiveUI)
	{
		CurrentActiveUI->RemoveFromParent();
		CurrentActiveUI = nullptr;
	}

	UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
	if (!GI) return;

	TSubclassOf<UCSUIBaseWidget> UIClassToCreate = nullptr;
	EMatchType CurrentMatchType = GI->GetMatchType();

	switch (CurrentMatchType)
	{
		case EMatchType::EMT_MainMenu:
			UIClassToCreate = MainMenuWidgetClass;
			break;
		case EMatchType::EMT_Single:
		case EMatchType::EMT_Versus:
		case EMatchType::EMT_Coop:
		//	case EMT_Tutorial:
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Invalid MatchType: %d"), static_cast<int32>(CurrentMatchType));
			break;
	}

	if (UIClassToCreate)
	{
		CurrentActiveUI = CreateWidget<UCSUIBaseWidget>(this, UIClassToCreate);
		
		if (CurrentActiveUI)
		{
			CurrentActiveUI->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CurrentActiveUI is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UIClassToCreate is nullptr"));
	}
}

void ACSPlayerController::Client_ShowLobbyUI_Implementation()
{
	if (IsLocalController())
	{
		if (CurrentActiveUI)
		{
			CurrentActiveUI->RemoveFromParent();
			CurrentActiveUI = nullptr;

			if (LobbyWidgetInstance != nullptr && LobbyWidgetInstance->IsInViewport()) return;

			LobbyWidgetInstance = CreateWidget<UCSUIBaseWidget>(this, LobbyWidgetClass);

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
}

void ACSPlayerController::UpdateTutorialObjectiveUI(const FText& ObjectiveText)
{
	if (IsLocalController() && CurrentActiveUI)
	{
		//CurrentActiveUI->SetObjectiveText(ObjectiveText);
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
		LobbyGameMode->ReturnToMainMenu(this);
	}
}

void ACSPlayerController::Client_OnSuddenDeath_Implementation()
{
	if (IsLocalController() && CurrentActiveUI)
	{
		CurrentActiveUI->OnSuddenDeathUI();
	}
}

void ACSPlayerController::UpdateReadyUI(bool bReady)
{
	if (IsLocalController() && CurrentActiveUI)
	{
		CurrentActiveUI->UpdateReadyUI(bReady);
	}
}

void ACSPlayerController::UpdateTeamUI(int32 TeamID)
{
	if (IsLocalController() && CurrentActiveUI)
	{
		CurrentActiveUI->UpdateTeamUI(TeamID);
	}
}

void ACSPlayerController::UpdateCharacterUI(FName SelectedCharacterID)
{
	if (IsLocalController() && CurrentActiveUI)
	{
		CurrentActiveUI->UpdateCharacterUI(SelectedCharacterID);
	}
}

void ACSPlayerController::UpdateSelectedMapUI(FName SelectedMap)
{
	if (IsLocalController() && CurrentActiveUI)
	{
		CurrentActiveUI->UpdateSelectedMapUI(SelectedMap);
	}
}

void ACSPlayerController::UpdateMatchTimeUI(int32 Time)
{
	if (IsLocalController() && CurrentActiveUI)
	{
		CurrentActiveUI->UpdateMatchTimeUI(Time);
	}
}

void ACSPlayerController::OnMatchPhaseChanged(EMatchPhase MatchPhase)
{
	if (IsLocalController())
	{
		if (CurrentActiveUI)
		{
			UCSGameInstance* Gl = GetGameInstance<UCSGameInstance>();
			ACSGameStateBase* CSGameState = GetWorld()->GetGameState<ACSGameStateBase>();
			
			if (Gl && CSGameState)
			{
				EMatchType CurrentMatchType = Gl->GetMatchType();

				switch (MatchPhase)
				{
				case EMatchPhase::EMP_None:
					UE_LOG(LogTemp, Warning, TEXT("MatchPhase is None"));
					break;
				case EMatchPhase::EMP_Waiting:
					UE_LOG(LogTemp, Warning, TEXT("MatchPhase is Waiting"));
					break;
				case EMatchPhase::EMP_Playing:
					UE_LOG(LogTemp, Warning, TEXT("MatchPhase is Playing"));
					break;
				case EMatchPhase::EMP_Finished:
					UE_LOG(LogTemp, Warning, TEXT("MatchPhase is Finished"));
					break;
				default:
					UE_LOG(LogTemp, Warning, TEXT("MatchPhase is Invalid"));
					break;

					//TODO: CurrentActiveUI->Show Each Match PhaseUI(MatchPhase);
				}
			}
		}
	}
}

void ACSPlayerController::SetPlayerRole(int PlayerRole)
{
	CharacterRole = PlayerRole;
}

void ACSPlayerController::HealthUpdate(float Health, float MaxHealth)
{
	UE_LOG(LogTemp, Warning, TEXT("Client: %s - HealthUpdate Called. Health: %.1f, MaxHealth: %.1f"), *GetName(), Health, MaxHealth);

	if (IsLocalController())
	{	
		//TODO : PlayerHUD->UpdateHealth(Health, MaxHealth);
	}
}
