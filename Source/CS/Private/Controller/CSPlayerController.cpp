// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CSPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/CSLobbyGameMode.h"
#include "GameModes/CSMainMenuGameMode.h"
#include "PlayerStates/CSPlayerState.h"
#include "GameStates/CSLobbyGameState.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "GameStates/CSGameStateBase.h"
#include "UI/CSUIBaseWidget.h"
#include "UI/CSMainMenu.h"
#include "UI/CSLobbyBaseWidget.h"
#include "UI/CSVersusLobbyWidget.h"
#include "UI/CSCoopLobbyWidget.h"
#include "UI/CSInGameHUD.h"
#include "Kismet/GameplayStatics.h"

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

    UE_LOG(LogTemp, Warning, TEXT("ACSPlayerController::ClientRestart --- Called for Controller: %s"), *GetName());

    if (!IsLocalPlayerController()) {
        UE_LOG(LogTemp, Log, TEXT("ClientRestart: Not Local Player Controller. Skipping UI/Input setup."));
        return;
    }

    UWorld* World = GetWorld();
    UCSAdvancedGameInstance* GI = GetGameInstance<UCSAdvancedGameInstance>();
    if (!GI || !World) { UE_LOG(LogTemp, Error, TEXT("ClientRestart: GI or World is NULL!")); return; }

    FName CurrentLevelName = FName(*World->GetName());
    UE_LOG(LogTemp, Log, TEXT("ClientRestart: Level=%s"), *CurrentLevelName.ToString());

    // 메인 메뉴나 로비 레벨에서는 실행 안 함
    if (CurrentLevelName == FName("MainMenuLevel") || CurrentLevelName == FName("LobbyLevel")) { return; }

    UE_LOG(LogTemp, Warning, TEXT("ClientRestart: In Gameplay Level. Setting up HUD and Game Input Mode..."));

    // 기존 UI 제거
    if (CurrentActiveUI) { CurrentActiveUI->RemoveFromParent(); CurrentActiveUI = nullptr; }

    // 레벨에 맞는 HUD 클래스 결정
    TSubclassOf<UCSUIBaseWidget> UIClassToCreate = nullptr;
    // ... (레벨 이름으로 HUD 클래스 결정 로직 - 이전과 동일) ...
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
            UE_LOG(LogTemp, Log, TEXT("ClientRestart: Initialized Gameplay UI: %s"), *UIClassToCreate->GetName());
        }
        else { UE_LOG(LogTemp, Error, TEXT("ClientRestart: CreateWidget FAILED!")); }
    }
    else { UE_LOG(LogTemp, Warning, TEXT("ClientRestart: No HUD Class determined!")); }

    // 입력 모드 설정 (GameOnly)
    FInputModeGameOnly InputModeData;
    SetInputMode(InputModeData);
    bShowMouseCursor = false;
    UE_LOG(LogTemp, Warning, TEXT("ClientRestart: >>> Input Mode SET TO GAME ONLY <<<"));

    // 뷰포트 포커스 설정
    FSlateApplication::Get().SetUserFocusToGameViewport(0);
    UE_LOG(LogTemp, Log, TEXT("ClientRestart: >>> Set Focus to Game Viewport <<<"));
}

// InitializeCurrentUI 함수 단순화: 메인 메뉴만 처리
void ACSPlayerController::InitializeCurrentUI()
{
    UWorld* World = GetWorld();
    UCSAdvancedGameInstance* GI = GetGameInstance<UCSAdvancedGameInstance>();
    if (!GI || !World) return;

    FName CurrentLevelName = FName(*World->GetName());
    UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: Checking Level=%s"), *CurrentLevelName.ToString());

    // --- 로비 레벨 예외 처리 유지 ---
    if (CurrentLevelName == FName("LobbyLevel")) {
        UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: In LobbyLevel, skipping UI creation/removal logic."));
        // 로비 진입 시 입력 모드는 Client_ShowLobbyUI 에서 처리
        return;
    }
    // --- 예외 처리 끝 ---

    // 다른 레벨에서 혹시 남아있을 수 있는 UI 제거
    if (CurrentActiveUI) {
        CurrentActiveUI->RemoveFromParent();
        CurrentActiveUI = nullptr;
    }

    // 메인 메뉴 레벨일 경우에만 메인 메뉴 UI 로드
    if (CurrentLevelName == FName("MainMenuLevel")) {
        TSubclassOf<UCSUIBaseWidget> UIClassToCreate = MainMenuWidgetClass;
        if (UIClassToCreate) {
            CurrentActiveUI = CreateWidget<UCSUIBaseWidget>(this, UIClassToCreate);
            if (CurrentActiveUI) {
                CurrentActiveUI->AddToViewport();
                UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: Initialized UI: %s"), *UIClassToCreate->GetName());
                // 메인 메뉴 입력 모드 설정
                FInputModeUIOnly InputModeData;
                InputModeData.SetWidgetToFocus(CurrentActiveUI->TakeWidget()); // 메인 메뉴에 포커스
                SetInputMode(InputModeData);
                bShowMouseCursor = true;
                UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: Set Input Mode to UIOnly for MainMenu."));
            }
            else { UE_LOG(LogTemp, Error, TEXT("InitializeCurrentUI: Failed to create MainMenuWidget!")); }
        }
        else { UE_LOG(LogTemp, Warning, TEXT("InitializeCurrentUI: MainMenuWidgetClass is not set!")); }
    }
    else {
        UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: Not MainMenuLevel, UI will be handled by OnPossess or Client_ShowLobbyUI."));
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
        // LobbyGameMode의 SetPlayerSelection 함수 시그니처도 EJobTypes 로 변경 필요
        // 확인 후 호출 또는 주석 처리
        LobbyGameMode->SetSelectedPlayerJob(this, SelectedJob);
    }
}

bool ACSPlayerController::Server_RequestReady_Validate(bool bReady) { return true; }
void ACSPlayerController::Server_RequestReady_Implementation(bool bReady)
{
    ACSPlayerState* PS = GetPlayerState<ACSPlayerState>();
    if (PS) { PS->SetIsReady(bReady); }
    else { UE_LOG(LogTemp, Warning, TEXT("Server_RequestReady: PlayerState not found.")); }
}

bool ACSPlayerController::Server_RequestReturnToMainMenu_Validate() { return true; }
void ACSPlayerController::Server_RequestReturnToMainMenu_Implementation()
{
    // TODO: 메인 메뉴 레벨로 이동
    UWorld* World = GetWorld();
    if (World) {
        UE_LOG(LogTemp, Log, TEXT("Server: Returning to Main Menu..."));
        // UGameplayStatics::OpenLevel(World, FName("MainMenuLevel")); // 클라이언트에게는 적용 안 됨
        // 모든 클라이언트에게 이동 명령 필요
        // GetWorld()->ServerTravel(TEXT("/Game/Maps/MainMenuLevel?listen")); // 리슨 서버 호스트만 이동
        // TODO: 각 클라이언트가 스스로 MainMenuLevel 로 이동하도록 처리 (예: GameMode에서 RPC 호출)
        AGameModeBase* GM = World->GetAuthGameMode();
        // if (GM) GM->RequestClientsReturnToMainMenu(); // GameMode에 함수 추가 필요
    }
}

void ACSPlayerController::Client_OnSuddenDeath_Implementation()
{
    if (IsLocalController() && CurrentActiveUI) {
        // CurrentActiveUI->TriggerSuddenDeathUI(); // 위젯 함수 호출
    }
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