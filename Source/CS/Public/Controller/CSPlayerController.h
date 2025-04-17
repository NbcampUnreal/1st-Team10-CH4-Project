// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSTypes/CSGameTypes.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSPlayerController.generated.h"

// 전방 선언
class UCSUIBaseWidget;
class UCSVersusLobbyWidget;
class UCSCoopLobbyWidget;
class APawn; // 추가
class UCSMainMenu;
class UCSLobbyBaseWidget;
class UCSInGameHUD;

UCLASS()
class CS_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACSPlayerController();

	// 유지되는 함수들
	void HealthUpdate(float Health, float MaxHealth);
	void OnMatchPhaseChanged(EMatchPhase MatchPhase);

	UFUNCTION(BlueprintPure, Category = "UI")
	UCSUIBaseWidget* GetCurrentUI() const { return CurrentActiveUI; }

	/** RPCs */
	UFUNCTION(Client, Reliable)
	void Client_ShowLobbyUI();
	virtual void Client_ShowLobbyUI_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_ShowNoSessionPopup();
	virtual void Client_ShowNoSessionPopup_Implementation();

	// MainMenu -> GameInstance 호출용 RPC 제거됨

	// 로비 상호작용 RPCs
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestTeamChange();
	virtual bool Server_RequestTeamChange_Validate();
	virtual void Server_RequestTeamChange_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SelectCharacter(EJobTypes SelectedJob);
	virtual bool Server_SelectCharacter_Validate(EJobTypes SelectedJob);
	virtual void Server_SelectCharacter_Implementation(EJobTypes SelectedJob);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestReady(bool bReady);
	virtual bool Server_RequestReady_Validate(bool bReady);
	virtual void Server_RequestReady_Implementation(bool bReady);

	// 메인 메뉴 복귀 RPC
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestReturnToMainMenu();
	virtual bool Server_RequestReturnToMainMenu_Validate();
	virtual void Server_RequestReturnToMainMenu_Implementation();

	// 서든데스 RPC
	UFUNCTION(Client, Reliable)
	void Client_OnSuddenDeath();
	virtual void Client_OnSuddenDeath_Implementation();

	// 사망 후 시점 전환
	UFUNCTION(Client, Reliable)
	void Client_SpectateTarget(APawn* NewTarget);
	void Client_SpectateTarget_Implementation(APawn* NewTarget);

protected:
	virtual void BeginPlay() override;
	virtual void ClientRestart_Implementation(APawn* NewPawn) override;

	bool bHasInitializedUI = false;

	// 현재 상태 맞는 기본 UI 로드
	void InitializeCurrentUI();

	/** 위젯 클래스 참조 */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCSUIBaseWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (DisplayName = "Versus Lobby Widget Class"))
	TSubclassOf<UCSVersusLobbyWidget> VersusLobbyWidgetClass; // 대전 로비 BP

	UPROPERTY(EditDefaultsOnly, Category = "UI", meta = (DisplayName = "Coop Lobby Widget Class"))
	TSubclassOf<UCSCoopLobbyWidget> CoopLobbyWidgetClass;   // 협동 로비 BP

	// HUD 클래스들
	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UCSUIBaseWidget> StageHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UCSUIBaseWidget> BossHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UCSUIBaseWidget> VersusHUDClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI") TSubclassOf<UCSUIBaseWidget> CoopHUDClass;

	// 현재 활성화된 UI 위젯 포인터
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCSUIBaseWidget> CurrentActiveUI; // TObjectPtr 사용
};
