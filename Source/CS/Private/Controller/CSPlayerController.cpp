#include "Controller/CSPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameModes/CSGameModeBase.h"
#include "GameModes/CSLobbyGameMode.h"
#include "PlayerStates/CSPlayerState.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "GameStates/CSGameStateBase.h"
#include "UI/CSUIBaseWidget.h"
#include "UI/CSMainMenu.h"
#include "UI/CSLobbyBaseWidget.h"
#include "UI/CSVersusLobbyWidget.h"
#include "UI/CSCoopLobbyWidget.h"
#include "UI/CSInGameHUD.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Engine/LocalPlayer.h"

ACSPlayerController::ACSPlayerController() : CurrentActiveUI(nullptr), bIsHostPlayer(false) {}


void ACSPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (IsLocalController())
    {
        InitializeCurrentUI();
    }
}

void ACSPlayerController::ClientRestart_Implementation(APawn* NewPawn)
{
    Super::ClientRestart_Implementation(NewPawn);


    if (!IsLocalPlayerController()) {
        return;
    }

    UWorld* World = GetWorld();
    UCSAdvancedGameInstance* GI = GetGameInstance<UCSAdvancedGameInstance>();
    if (!GI || !World) { UE_LOG(LogTemp, Error, TEXT("ClientRestart: GI or World is NULL!")); return; }

    FName CurrentLevelName = FName(*World->GetName());
    UE_LOG(LogTemp, Log, TEXT("ClientRestart: Level=%s"), *CurrentLevelName.ToString());

    // 메인 메뉴나 로비 레벨에서는 실행 안 함
    if (CurrentLevelName == FName("MainMenuLevel") || CurrentLevelName == FName("LobbyLevel")) { return; }


    if (CurrentActiveUI) { CurrentActiveUI->RemoveFromParent(); CurrentActiveUI = nullptr; }

    TSubclassOf<UCSUIBaseWidget> UIClassToCreate = nullptr;

    if (CurrentLevelName == FName("SingleModeLevel")) { UIClassToCreate = StageHUDClass; }
    else if (CurrentLevelName == FName("SingleModeBossLevel")) { UIClassToCreate = BossHUDClass; }
    else if (CurrentLevelName == FName("VersusModeLevel")) { UIClassToCreate = VersusHUDClass; }
    else if (CurrentLevelName == FName("CoopModeLevel")) { UIClassToCreate = CoopHUDClass; }
    else { UE_LOG(LogTemp, Warning, TEXT("ClientRestart: Unknown Gameplay Level %s"), *CurrentLevelName.ToString()); }

    // HUD 생성 및 표시
    if (UIClassToCreate) {
        CurrentActiveUI = CreateWidget<UCSUIBaseWidget>(this, UIClassToCreate);
        if (CurrentActiveUI) {
            CurrentActiveUI->AddToViewport();
        }
    }

    // 입력 모드 설정 (GameOnly)
    FInputModeGameOnly InputModeData;
    SetInputMode(InputModeData);
    bShowMouseCursor = false;

    // 뷰포트 포커스 설정
    FSlateApplication::Get().SetUserFocusToGameViewport(0);
}

void ACSPlayerController::InitializeCurrentUI()
{
    UWorld* World = GetWorld();
    UCSAdvancedGameInstance* GI = GetGameInstance<UCSAdvancedGameInstance>();
    if (!GI || !World) return;

    FName CurrentLevelName = FName(*World->GetName());

    if (CurrentLevelName == FName("LobbyLevel")) {
        return;
    }

    if (CurrentActiveUI) {
        CurrentActiveUI->RemoveFromParent();
        CurrentActiveUI = nullptr;
    }

    if (CurrentLevelName == FName("MainMenuLevel")) {
        TSubclassOf<UCSUIBaseWidget> UIClassToCreate = MainMenuWidgetClass;
        if (UIClassToCreate) {
            CurrentActiveUI = CreateWidget<UCSUIBaseWidget>(this, UIClassToCreate);
            if (CurrentActiveUI) {
                CurrentActiveUI->AddToViewport();
                UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: Initialized UI: %s"), *UIClassToCreate->GetName());

                FInputModeUIOnly InputModeData;
                InputModeData.SetWidgetToFocus(CurrentActiveUI->TakeWidget());
                SetInputMode(InputModeData);
                bShowMouseCursor = true;
            }
        }
    }
}

void ACSPlayerController::Client_ShowLobbyUI_Implementation()
{
    if (!IsLocalController()) return;
    if (CurrentActiveUI) { CurrentActiveUI->RemoveFromParent(); CurrentActiveUI = nullptr; }

    UCSAdvancedGameInstance* GI = GetGameInstance<UCSAdvancedGameInstance>();

    EMatchType CurrentMatchType = GI->GetMatchType();
    TSubclassOf<UCSLobbyBaseWidget> LobbyClassToCreate = nullptr;

    if (CurrentMatchType == EMatchType::EMT_Versus) {
        LobbyClassToCreate = VersusLobbyWidgetClass;
    }
    else if (CurrentMatchType == EMatchType::EMT_Coop) {
        LobbyClassToCreate = CoopLobbyWidgetClass;
    }

    if (LobbyClassToCreate) {
        UCSLobbyBaseWidget* LobbyWidget = CreateWidget<UCSLobbyBaseWidget>(this, LobbyClassToCreate);
        if (LobbyWidget) {
            CurrentActiveUI = LobbyWidget;
            CurrentActiveUI->AddToViewport();
            LobbyWidget->InitializeLobby(CurrentMatchType); // 초기화 함수 호출
            // UI 입력 모드 설정
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(CurrentActiveUI->TakeWidget());
            SetInputMode(InputModeData);
            bShowMouseCursor = true;
        }
    }
}

void ACSPlayerController::Client_ShowNoSessionPopup_Implementation()
{
    if (IsLocalController()) {
        InitializeCurrentUI();
        UCSMainMenu* MainMenuWidget = Cast<UCSMainMenu>(GetCurrentUI());
        if (MainMenuWidget) {
            MainMenuWidget->ShowNotification(FText::FromString(TEXT("[호스트가 세션을 생성하지 않았습니다.]")));
        }
    }
}

bool ACSPlayerController::Server_RequestTeamChange_Validate() { return true; }
void ACSPlayerController::Server_RequestTeamChange_Implementation()
{
    ACSLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ACSLobbyGameMode>();
    if (LobbyGameMode) { LobbyGameMode->ChangeTeam(this); }
}

bool ACSPlayerController::Server_SelectCharacter_Validate(EJobTypes SelectedJob) { return true; }
void ACSPlayerController::Server_SelectCharacter_Implementation(EJobTypes SelectedJob)
{
    ACSLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ACSLobbyGameMode>();
    if (LobbyGameMode) {
        LobbyGameMode->SetSelectedPlayerJob(this, SelectedJob);
    }
}

bool ACSPlayerController::Server_RequestReady_Validate(bool bReady) { return true; }
void ACSPlayerController::Server_RequestReady_Implementation(bool bReady)
{
    ACSPlayerState* PS = GetPlayerState<ACSPlayerState>();
    if (PS) { PS->SetIsReady(bReady); }
}

bool ACSPlayerController::Server_RequestReturnToMainMenu_Validate() { return true; }
void ACSPlayerController::Server_RequestReturnToMainMenu_Implementation()
{
    UWorld* World = GetWorld();
    if (!World) {
        return;
    }

    AGameModeBase* CurrentGM = UGameplayStatics::GetGameMode(World);

    if (!CurrentGM) {
        return; // GameMode 없으면 진행 불가
    }
    if (ACSLobbyGameMode* LobbyGM = Cast<ACSLobbyGameMode>(CurrentGM)) {
        LobbyGM->ReturnAllPlayersToMainMenu();
    }
    else if (ACSGameModeBase* CastedGameModeBase = Cast<ACSGameModeBase>(CurrentGM)) {
        CastedGameModeBase->ReturnAllPlayersToMainMenu();
    }
    else {
    }
}

void ACSPlayerController::Client_OnSuddenDeath_Implementation()
{
    if (IsLocalController() && CurrentActiveUI) {
        // CurrentActiveUI->TriggerSuddenDeathUI(); // 위젯 함수 호출
    }
}

void ACSPlayerController::Client_SpectateTarget_Implementation(APawn* NewTarget)
{
    if (!NewTarget) return;

    SetViewTargetWithBlend(NewTarget, 0.5f);
}

void ACSPlayerController::HealthUpdate(float Health, float MaxHealth)
{
    if (IsLocalController() && CurrentActiveUI) {
        if (UCSInGameHUD* HUD = Cast<UCSInGameHUD>(CurrentActiveUI)) {
            HUD->UpdateHealth(Health, MaxHealth);
        }
    }
}

// 매치 단계 변경
void ACSPlayerController::OnMatchPhaseChanged(EMatchPhase MatchPhase)
{
    if (IsLocalController() && CurrentActiveUI) {
        // CurrentActiveUI->HandleMatchPhaseChanged(MatchPhase); // 위젯 함수 호출
    }
}