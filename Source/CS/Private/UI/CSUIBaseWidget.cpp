#include "UI/CSUIBaseWidget.h"
#include "Controller/CSPlayerController.h"
#include "PlayerStates/CSPlayerState.h"
#include "Kismet/GameplayStatics.h" // 추가

// C++ 기본 구현 (_Implementation 함수 정의는 유지)
void UCSUIBaseWidget::UpdateReadyStatus_Implementation(bool bIsReady) { /* BP에서 구현 */ }
void UCSUIBaseWidget::UpdateTeamDisplay_Implementation(int32 TeamID) { /* BP에서 구현 */ }
void UCSUIBaseWidget::UpdateCharacterDisplay_Implementation(EJobTypes NewJob) { /* BP에서 구현 */ }
void UCSUIBaseWidget::UpdateHealth_Implementation(float CurrentHealth, float MaxHealth) { /* BP에서 구현하거나 여기서 기본 구현 */ }
void UCSUIBaseWidget::UpdateMatchTimer_Implementation(int32 Time) { /* BP에서 구현 */ }
void UCSUIBaseWidget::HandleMatchPhaseChanged_Implementation(EMatchPhase NewPhase) { /* BP에서 구현 */ }
void UCSUIBaseWidget::TriggerSuddenDeathUI_Implementation() { /* BP에서 구현 */ }

// Helper function 구현
ACSPlayerController* UCSUIBaseWidget::GetOwningCSPlayerController() const {
	// GameplayStatics 헤더 필요
	return Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(this, 0)); // GetOwningPlayer() 대신 사용
}
ACSPlayerState* UCSUIBaseWidget::GetOwningCSPlayerState() const {
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0); // GetOwningPlayer() 대신 사용
	return PC ? Cast<ACSPlayerState>(PC->PlayerState) : nullptr;
}