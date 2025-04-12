// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CSPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "GameModes/CSLobbyGameMode.h"
#include "GameModes/CSMainMenuGameMode.h"
#include "PlayerStates/CSPlayerState.h"
#include "GameStates/CSLobbyGameState.h"
#include "GameInstance/CSGameInstance.h"
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

void ACSPlayerController::InitializeCurrentUI()
{
    UWorld* World = GetWorld();
    UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
    if (!GI || !World) return;

    FName CurrentLevelName = FName(*World->GetName());
    EMatchType CurrentMatchType = GI->GetMatchType();
    UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: Checking Level=%s, MatchType=%d"), *CurrentLevelName.ToString(), (int32)CurrentMatchType);

    if (CurrentLevelName == FName("LobbyLevel"))
    {
        UE_LOG(LogTemp, Log, TEXT("InitializeCurrentUI: In LobbyLevel, skipping UI creation/removal logic."));
        FInputModeUIOnly InputModeData;
        SetInputMode(InputModeData);
        bShowMouseCursor = true;
        return;
    }
    if (CurrentActiveUI) {
        CurrentActiveUI->RemoveFromParent();
        CurrentActiveUI = nullptr;
    }

    TSubclassOf<UCSUIBaseWidget> UIClassToCreate = nullptr;

    if (CurrentLevelName == FName("MainMenuLevel")) {
        UIClassToCreate = MainMenuWidgetClass;
    }
    else if (CurrentLevelName == FName("SingleModeLevel")) {
        UIClassToCreate = StageHUDClass;
    }
    else if (CurrentLevelName == FName("SingleModeBossLevel")) {
        UIClassToCreate = BossHUDClass;
    }
    else if (CurrentLevelName == FName("VersusModeLevel")) {
        UIClassToCreate = VersusHUDClass;
    }
    else if (CurrentLevelName == FName("CoopModeLevel")) {
        UIClassToCreate = CoopHUDClass;
    }

    if (UIClassToCreate) {
        CurrentActiveUI = CreateWidget<UCSUIBaseWidget>(this, UIClassToCreate);
        if (CurrentActiveUI) {
            CurrentActiveUI->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("Initialized UI: %s"), *UIClassToCreate->GetName());

            bool bIsUIInputMode = (CurrentLevelName == FName("MainMenuLevel"));
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
                FSlateApplication::Get().SetUserFocusToGameViewport(0);
            }
        }
    }
}

void ACSPlayerController::Client_ShowLobbyUI_Implementation()
{
    if (!IsLocalController()) return;
    if (CurrentActiveUI) { CurrentActiveUI->RemoveFromParent(); CurrentActiveUI = nullptr; }

    UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();

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

void ACSPlayerController::RequestEnterMultiplayerMode(EMatchType NewMatchType)
{
    if (UCSGameInstance* GameInstance = GetGameInstance<UCSGameInstance>())
    {
        UE_LOG(LogTemp, Warning, TEXT("🟩 RequestEnterMultiplayerMode | Player: %s | NetMode: %d | IsHost: %d"),
            *GetName(), (int32)GetNetMode(), bIsHostPlayer);

        if (bIsHostPlayer) // Listen Server
        {
            GameInstance->SetMatchType(NewMatchType);
            GameInstance->HostSession(NewMatchType);
        }
        else // Client
        {
            GameInstance->SetMatchType(NewMatchType);
            GameInstance->FindSessions();
        }
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