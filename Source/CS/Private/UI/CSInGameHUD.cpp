#include "UI/CSInGameHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Internationalization/Text.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameStates/CSGameStateBase.h"

void UCSInGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateHealth_Implementation(100.f, 100.f);
	UpdateMatchTimer_Implementation(0);

	HostTimerUpdateInterval = 0.5f;
	TimeSinceLastHostTimerUpdate = HostTimerUpdateInterval;

	CachedGameState = GetWorld() ? Cast<ACSGameStateBase>(GetWorld()->GetGameState()) : nullptr;
}

void UCSInGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	
	APlayerController* PC = GetOwningPlayer();
	if (PC && PC->HasAuthority())
	{
		TimeSinceLastHostTimerUpdate += InDeltaTime;
		if (TimeSinceLastHostTimerUpdate >= HostTimerUpdateInterval)
		{
			TimeSinceLastHostTimerUpdate = 0.0f;

			if (!CachedGameState.IsValid()) {
				CachedGameState = GetWorld() ? Cast<ACSGameStateBase>(GetWorld()->GetGameState()) : nullptr;
			}

			if (CachedGameState.IsValid()) {
				int32 CurrentRemainingTime = CachedGameState->GetRemainingMatchTime();
				UpdateMatchTimer(CurrentRemainingTime);
			}
		}
	}
}

void UCSInGameHUD::UpdateHealth_Implementation(float CurrentHealth, float MaxHealth)
{
	if (HealthBar) {
		float Percent = (MaxHealth > KINDA_SMALL_NUMBER) ? (CurrentHealth / MaxHealth) : 0.0f;
		HealthBar->SetPercent(Percent);
	}
	if (HealthText) {
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);

		FText HealthValueText = FText::AsNumber(CurrentHealth, &Opts);
		FText MaxHealthValueText = FText::AsNumber(MaxHealth, &Opts);
		HealthText->SetText(FText::Format(NSLOCTEXT("CSInGameHUD", "HealthFormat", "{0} / {1}"), HealthValueText, MaxHealthValueText));
	}
}

void UCSInGameHUD::UpdateMatchTimer_Implementation(int32 RemainingSeconds)
{
	if (MatchTimerText) {
		int32 Minutes = FMath::Max(0, RemainingSeconds) / 60;
		int32 Seconds = FMath::Max(0, RemainingSeconds) % 60;
		FString TimerString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		MatchTimerText->SetText(FText::FromString(TimerString));
	}
}