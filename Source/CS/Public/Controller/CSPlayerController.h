// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSTypes/CSGameTypes.h"
#include "CSPlayerController.generated.h"

class UUserWidget;

UCLASS()
class CS_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACSPlayerController();
	void SetPlayerRole(int PlayerRole);
	void HealthUpdate(float Health, float MaxHealth);

	void UpdateCharacterUI(FName SelectedCharacterID);
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

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_RequestTeamChange();
	virtual bool Server_RequestTeamChange_Validate();
	virtual void Server_RequestTeamChange_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SelectCharacter(FName CharacterID);
	virtual bool Server_SelectCharacter_Validate(FName CharacterID);
	virtual void Server_SelectCharacter_Implementation(FName CharacterID);

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


	/*
	*		UI
	*/



protected:
	virtual void BeginPlay() override;

	void InitMatchUI();

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;

	UPROPERTY()
	UUserWidget* LobbyWidgetInstance;

	UPROPERTY()
	UUserWidget* MainWidgetInstance;



private:
	int32 CharacterRole;

	//UPROPERTY()
	//class UHUDWidget* PlayerHUD;
	
};
