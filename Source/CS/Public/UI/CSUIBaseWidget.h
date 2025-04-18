#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CSTypes/CSCharacterTypes.h" // EJobTypes 포함
#include "CSTypes/CSGameTypes.h"     // EMatchPhase 포함
#include "CSUIBaseWidget.generated.h"

UCLASS(Abstract)
class CS_API UCSUIBaseWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateReadyStatus(bool bIsReady);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateTeamDisplay(int32 TeamID);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateCharacterDisplay(EJobTypes NewJob);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateMatchTimer(int32 Time);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void HandleMatchPhaseChanged(EMatchPhase NewPhase);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void TriggerSuddenDeathUI();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI Notification")
    void ShowNotificationMessage(const FText& Message);

protected:
    UFUNCTION(BlueprintPure, Category = "CS Base Widget")
    class ACSPlayerController* GetOwningCSPlayerController() const;

    UFUNCTION(BlueprintPure, Category = "CS Base Widget")
    class ACSPlayerState* GetOwningCSPlayerState() const;
};