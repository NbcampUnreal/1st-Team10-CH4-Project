// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CSPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/CSLobbyGameMode.h"
#include "GameModes/CSMainMenuGameMode.h" // 메인 메뉴 GameMode (RPC에서 사용)
#include "PlayerStates/CSPlayerState.h"
#include "GameStates/CSLobbyGameState.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSGameStateBase.h"
#include "UI/CSUIBaseWidget.h"
#include "UI/CSUIBaseWidget.h"
#include "UI/CSMainMenu.h"
#include "UI/CSLobbyBaseWidget.h"
#include "UI/CSVersusLobbyWidget.h"
#include "UI/CSCoopLobbyWidget.h"
#include "UI/CSInGameHUD.h" // HUD 베이스 클래스 예시
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h" // GetWorld()


ACSPlayerController::ACSPlayerController() : CurrentActiveUI(nullptr) {} // 초기화

void ACSPlayerController::BeginPlay()
{
    Super::BeginPlay();
    if (IsLocalController()) {
        InitializeCurrentUI();
    }
}

void ACSPlayerController::InitializeCurrentUI()
{
    if (CurrentActiveUI) {
        CurrentActiveUI->RemoveFromParent();
        CurrentActiveUI = nullptr;
    }

    UWorld* World = GetWorld();
    UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
    // AGameStateBase* GS = World ? World->GetGameState() : nullptr; // GameState 필요 시 사용
    if (!GI || !World) return;

    TSubclassOf<UCSUIBaseWidget> UIClassToCreate = nullptr;
    EMatchType CurrentMatchType = GI->GetMatchType();
    FName CurrentLevelName = FName(*World->GetName());

    UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: MatchType=%d, Level=%s"), (int32)CurrentMatchType, *CurrentLevelName.ToString());

    if (CurrentLevelName == FName("MainMenuLevel")) { // 레벨 이름 확인
        UIClassToCreate = MainMenuWidgetClass;
    }
    else if (CurrentLevelName == FName("LobbyLevel")) { // 레벨 이름 확인
        UE_LOG(LogTemp, Log, TEXT("In Lobby Level, waiting for Client_ShowLobbyUI RPC."));
    }
    else if (CurrentLevelName == FName("StageLevel")) { // 레벨 이름 확인
        UIClassToCreate = StageHUDClass;
    }
    else if (CurrentLevelName == FName("BossLevel")) { // 레벨 이름 확인
        UIClassToCreate = BossHUDClass;
    }
    else if (CurrentLevelName == FName("VersusLevel")) { // 레벨 이름 확인
        UIClassToCreate = VersusHUDClass;
    }
    else if (CurrentLevelName == FName("CoopLevel")) { // 레벨 이름 확인
        UIClassToCreate = CoopHUDClass;
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("InitializeCurrentUI: Unknown level or no specific UI defined for Level=%s"), *CurrentLevelName.ToString());
        UIClassToCreate = MainMenuWidgetClass; // 기본값으로 메인 메뉴 로드
    }

    if (UIClassToCreate) {
        CurrentActiveUI = CreateWidget<UCSUIBaseWidget>(this, UIClassToCreate);
        if (CurrentActiveUI) {
            CurrentActiveUI->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("Initialized UI: %s"), *UIClassToCreate->GetName());
            // 입력 모드 설정
            bool bIsUIInputMode = (CurrentLevelName == FName("MainMenuLevel") || CurrentLevelName == FName("LobbyLevel"));
            if (bIsUIInputMode) {
                FInputModeUIOnly InputModeData;
                InputModeData.SetWidgetToFocus(CurrentActiveUI->TakeWidget());
                SetInputMode(InputModeData);
                bShowMouseCursor = true;
            }
            else {
                FInputModeGameOnly InputModeData;
                SetInputMode(InputModeData);
                bShowMouseCursor = false;
            }
        }
        else { UE_LOG(LogTemp, Error, TEXT("Failed to create widget for class %s"), *UIClassToCreate->GetName()); }
    }
    else { UE_LOG(LogTemp, Warning, TEXT("No UI class determined to create for current state.")); }
}

// 로비 UI 표시 (GameMode 등에서 호출)
void ACSPlayerController::Client_ShowLobbyUI_Implementation()
{
    if (!IsLocalController()) return;
    UE_LOG(LogTemp, Log, TEXT("Client_ShowLobbyUI_Implementation called."));
    if (CurrentActiveUI) { CurrentActiveUI->RemoveFromParent(); CurrentActiveUI = nullptr; }

    UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
    if (!GI) { UE_LOG(LogTemp, Error, TEXT("Client_ShowLobbyUI: GameInstance is null!")); return; }

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
            UE_LOG(LogTemp, Log, TEXT("Created and shown lobby widget: %s"), *LobbyClassToCreate->GetName());
        }
        else { UE_LOG(LogTemp, Error, TEXT("Failed to create lobby widget instance from class %s!"), *LobbyClassToCreate->GetName()); }
    }
    else { UE_LOG(LogTemp, Error, TEXT("Lobby widget class is null for MatchType %d! Check PlayerController BP/Defaults."), (int32)CurrentMatchType); }
}

// 세션 없음 알림 표시
void ACSPlayerController::Client_ShowNoSessionPopup_Implementation()
{
    if (IsLocalController()) {
        InitializeCurrentUI(); // 메인 메뉴 UI 다시 로드 (실패했으므로)
        UCSMainMenu* MainMenuWidget = Cast<UCSMainMenu>(GetCurrentUI());
        if (MainMenuWidget) {
            MainMenuWidget->ShowNotification(FText::FromString(TEXT("[호스트가 세션을 생성하지 않았습니다.]")));
            UE_LOG(LogTemp, Warning, TEXT("Showed 'No Session Found' notification."));
            // 버튼 재활성화 (메인 메뉴에서 처리할 수도 있음)
           // MainMenuWidget->EnableMultiplayerButtons();
        }
        else { UE_LOG(LogTemp, Warning, TEXT("Client_ShowNoSessionPopup: Current UI is not Main Menu after InitializeCurrentUI.")); }
    }
}

// 메인 메뉴 -> GameInstance 호출용 RPC
bool ACSPlayerController::Server_RequestFindOrCreateSession_Validate(EMatchType MatchType) { return true; }
void ACSPlayerController::Server_RequestFindOrCreateSession_Implementation(EMatchType MatchType)
{
    UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
    if (GI) {
        UE_LOG(LogTemp, Log, TEXT("Server: Received request to find or create session for type %d"), (int32)MatchType);
        GI->FindOrCreateSession(MatchType);
    }
    else { UE_LOG(LogTemp, Error, TEXT("Server_RequestFindOrCreateSession: GameInstance is null!")); }
}


// 기존 RPC 구현들 (UI 업데이트 함수 호출 제거됨)

bool ACSPlayerController::Server_RequestTeamChange_Validate() { return true; }
void ACSPlayerController::Server_RequestTeamChange_Implementation()
{
    ACSLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ACSLobbyGameMode>();
    if (LobbyGameMode) { LobbyGameMode->ChangeTeam(this); }
    else { UE_LOG(LogTemp, Warning, TEXT("Server_RequestTeamChange: LobbyGameMode not found.")); }
}

bool ACSPlayerController::Server_SelectCharacter_Validate(EJobTypes SelectedJob) { return true; }
void ACSPlayerController::Server_SelectCharacter_Implementation(EJobTypes SelectedJob)
{
    ACSLobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ACSLobbyGameMode>();
    if (LobbyGameMode) {
        // LobbyGameMode의 SetPlayerSelection 함수 시그니처도 EJobTypes 로 변경 필요
        // 확인 후 호출 또는 주석 처리
        LobbyGameMode->SetPlayerSelection(this, SelectedJob);
    }
    else { UE_LOG(LogTemp, Warning, TEXT("Server_SelectCharacter: LobbyGameMode not found.")); }
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

// 체력 업데이트
void ACSPlayerController::HealthUpdate(float Health, float MaxHealth)
{
    if (IsLocalController() && CurrentActiveUI) {
        // HUD 위젯으로 캐스팅 후 함수 호출
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