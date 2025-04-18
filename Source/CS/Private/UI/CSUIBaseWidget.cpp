#include "UI/CSUIBaseWidget.h"
#include "Controller/CSPlayerController.h"
#include "PlayerStates/CSPlayerState.h"
#include "Kismet/GameplayStatics.h"

void UCSUIBaseWidget::UpdateReadyStatus_Implementation(bool bIsReady) {}
void UCSUIBaseWidget::UpdateTeamDisplay_Implementation(int32 TeamID) {}
void UCSUIBaseWidget::UpdateCharacterDisplay_Implementation(EJobTypes NewJob) {}
void UCSUIBaseWidget::UpdateHealth_Implementation(float CurrentHealth, float MaxHealth) {}
void UCSUIBaseWidget::UpdateMatchTimer_Implementation(int32 Time) {}
void UCSUIBaseWidget::HandleMatchPhaseChanged_Implementation(EMatchPhase NewPhase) {}
void UCSUIBaseWidget::TriggerSuddenDeathUI_Implementation() {}

ACSPlayerController* UCSUIBaseWidget::GetOwningCSPlayerController() const {
	return Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}
ACSPlayerState* UCSUIBaseWidget::GetOwningCSPlayerState() const {
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	return PC ? Cast<ACSPlayerState>(PC->PlayerState) : nullptr;
}