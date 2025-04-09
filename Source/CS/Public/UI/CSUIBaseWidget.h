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
    // BlueprintNativeEvent 선언만 남깁니다. (_Implementation 선언 제거)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateReadyStatus(bool bIsReady);
    // virtual void UpdateReadyStatus_Implementation(bool bIsReady); // 제거

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateTeamDisplay(int32 TeamID);
    // virtual void UpdateTeamDisplay_Implementation(int32 TeamID); // 제거

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateCharacterDisplay(EJobTypes NewJob);
    // virtual void UpdateCharacterDisplay_Implementation(EJobTypes NewJob); // 제거

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateHealth(float CurrentHealth, float MaxHealth);
	// virtual void UpdateHealth_Implementation(float CurrentHealth, float MaxHealth); // 제거

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void UpdateMatchTimer(int32 Time);
    // virtual void UpdateMatchTimer_Implementation(int32 Time); // 제거

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void HandleMatchPhaseChanged(EMatchPhase NewPhase);
    // virtual void HandleMatchPhaseChanged_Implementation(EMatchPhase NewPhase); // 제거

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "UI Update")
    void TriggerSuddenDeathUI(); // 이전 이름 OnSuddenDeathUI 에서 변경 가정 (오류 로그 기반)
    // virtual void TriggerSuddenDeathUI_Implementation(); // 제거

    // 메인 메뉴 알림용 (ImplementableEvent는 _Implementation 없음)
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UI Notification")
    void ShowNotificationMessage(const FText& Message);

protected:
    UFUNCTION(BlueprintPure, Category = "CS Base Widget")
    class ACSPlayerController* GetOwningCSPlayerController() const;

    UFUNCTION(BlueprintPure, Category = "CS Base Widget")
    class ACSPlayerState* GetOwningCSPlayerState() const;
};