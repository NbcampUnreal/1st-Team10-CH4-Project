// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSUIBaseWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CS_API UCSUIBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
    void UpdateReadyUI(bool bIsReady);
    virtual void UpdateReadyUI_Implementation(bool bIsReady);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
    void UpdateTeamUI(int32 TeamID);
    virtual void UpdateTeamUI_Implementation(int32 TeamID);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
    void UpdateCharacterUI(FName SelectedCharacterID);
    virtual void UpdateCharacterUI_Implementation(FName SelectedCharacterID);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
    void UpdateSelectedMapUI(FName SelectedMap);
    virtual void UpdateSelectedMapUI_Implementation(FName SelectedMap);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
    void UpdateMatchTimeUI(int32 Time);
    virtual void UpdateMatchTimeUI_Implementation(int32 Time);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
    void OnMatchPhaseChanged();
    virtual void OnMatchPhaseChanged_Implementation();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "UI")
    void OnSuddenDeathUI();
    virtual void OnSuddenDeathUI_Implementation();
};
