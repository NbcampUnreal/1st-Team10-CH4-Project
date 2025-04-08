// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSTypes/CSGameTypes.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSPlayerController.generated.h"

class UCSUIBaseWidget;

UCLASS()
class CS_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACSPlayerController();
	void SetPlayerRole(int PlayerRole);
	void HealthUpdate(float Health, float MaxHealth);
	void UpdateTutorialObjectiveUI(const FText& ObjectiveText);
	void UpdateCharacterUI(EJobTypes NewJob);
	void UpdateSelectedMapUI(FName SelectedMap);
	void UpdateMatchTimeUI(int32 Time);
	void OnMatchPhaseChanged(EMatchPhase MatchPhase);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateReadyUI(bool bReady);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateTeamUI(int32 TeamID);


	/*
	*		RPC
	*/

	UFUNCTION(Client, Reliable)
	void Client_ShowLobbyUI();
	virtual void Client_ShowLobbyUI_Implementation();

	UFUNCTION(Client, Reliable)
	void Client_ShowNoSessionPopup();
	virtual void Client_ShowNoSessionPopup_Implementation();

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

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SelectMap(FName Map);
	virtual bool Server_SelectMap_Validate(FName Map);
	virtual void Server_SelectMap_Implementation(FName Map);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestReturnToMainMenu();
	virtual bool Server_RequestReturnToMainMenu_Validate();
	virtual void Server_RequestReturnToMainMenu_Implementation();
	
	UFUNCTION(Client, Reliable)
	void Client_OnSuddenDeath();
	virtual void Client_OnSuddenDeath_Implementation();

protected:
	virtual void BeginPlay() override;

	// Reset main UI from game status
	void InitMatchUI(); 


	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UCSUIBaseWidget> LobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCSUIBaseWidget> MainMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCSUIBaseWidget> TutorialWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCSUIBaseWidget> CoopWidgetClass;

	UPROPERTY()
	UCSUIBaseWidget* CoopWidgetInstance;
	UPROPERTY()
	UCSUIBaseWidget* TutorialWidgetInstance;
	UPROPERTY()
	UCSUIBaseWidget* LobbyWidgetInstance;
	UPROPERTY()
	UCSUIBaseWidget* MainMenuWidgetInstance;


	UPROPERTY(VisibleAnywhere, Category = "UI")
	UCSUIBaseWidget* CurrentActiveUI;

private:
	int32 CharacterRole;

	FORCEINLINE UCSUIBaseWidget* GetCurrentUI() const { return CurrentActiveUI; }
	
	//UPROPERTY()
	//class UHUDWidget* PlayerHUD;
	
};
