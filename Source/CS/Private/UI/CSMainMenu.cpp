#include "UI/CSMainMenu.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/CSAdvancedGameInstance.h"
#include "Controller/CSPlayerController.h"
#include "CSTypes/CSGameTypes.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/NetDriver.h"

void UCSMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (ArcadeModeButton) ArcadeModeButton->OnClicked.AddDynamic(this, &UCSMainMenu::OnArcadeModeClicked);
	if (ExitButton) ExitButton->OnClicked.AddDynamic(this, &UCSMainMenu::OnExitClicked);

	if (NotificationText) NotificationText->SetVisibility(ESlateVisibility::Collapsed);
}

void UCSMainMenu::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(NotificationTimerHandle);
	}
	Super::NativeDestruct();
}

void UCSMainMenu::OnArcadeModeClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Arcade Mode Clicked"));
	UCSAdvancedGameInstance* GI = GetGameInstance<UCSAdvancedGameInstance>();
	UWorld* World = GetWorld();
	if (GI && World) {
		GI->SetMatchType(EMatchType::EMT_Single);
		RemoveFromParent();
		UGameplayStatics::OpenLevel(World, FName("SingleModeLevel"));
		UE_LOG(LogTemp, Log, TEXT("Opening SingleModeLevel for Arcade Mode."));
	}
	else { UE_LOG(LogTemp, Error, TEXT("Failed to get GameInstance or World for Arcade Mode.")); }
}

void UCSMainMenu::ShowNotification(const FText& Message, float Duration)
{
	if (NotificationText)
	{
		NotificationText->SetText(Message);
		NotificationText->SetVisibility(ESlateVisibility::HitTestInvisible);

		UWorld* World = GetWorld();
		if (!World) return;

		World->GetTimerManager().ClearTimer(NotificationTimerHandle);
		if (Duration > 0.0f) {
			World->GetTimerManager().SetTimer(NotificationTimerHandle, this, &UCSMainMenu::HideNotification, Duration, false);
			EnableMultiplayerButtons();
		}
	}
}

void UCSMainMenu::HideNotification()
{
	if (NotificationText) {
		NotificationText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCSMainMenu::SetMultiplayerButtonsEnabled(bool bEnabled)
{
	if (VersusModeButton) VersusModeButton->SetIsEnabled(bEnabled);
	if (CoopModeButton) CoopModeButton->SetIsEnabled(bEnabled);
}

void UCSMainMenu::EnableMultiplayerButtons()
{
	SetMultiplayerButtonsEnabled(true);
}


void UCSMainMenu::OnExitClicked()
{
	UE_LOG(LogTemp, Log, TEXT("Exit Clicked"));
	// GetOwningPlayer() 대신 GameplayStatics 사용
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC) PC->ConsoleCommand("quit");
}

// 제거된 FindOrCreateMultiplayerSession 함수
// void UCSMainMenu::FindOrCreateMultiplayerSession(EMatchType MatchType) { ... }