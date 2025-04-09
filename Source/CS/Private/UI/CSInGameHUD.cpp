#include "UI/CSInGameHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Internationalization/Text.h" // FText::AsNumber 사용 위해

void UCSInGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
	// _Implementation 함수 직접 호출하도록 수정
	UpdateHealth_Implementation(100.f, 100.f);
	UpdateMatchTimer_Implementation(0);
}

void UCSInGameHUD::UpdateHealth_Implementation(float CurrentHealth, float MaxHealth)
{
	if (HealthBar) {
		float Percent = (MaxHealth > KINDA_SMALL_NUMBER) ? (CurrentHealth / MaxHealth) : 0.0f;
		HealthBar->SetPercent(Percent);
		// UE_LOG(LogTemp, Verbose, TEXT("HUD Health Updated: %.1f / %.1f (%.2f)"), CurrentHealth, MaxHealth, Percent); // 로그 레벨 조정
	}
	if (HealthText) {
		// Conv_FloatToText 대체 -> FText::AsNumber 사용
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0); // 소수점 이하 0자리

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
		// UE_LOG(LogTemp, Verbose, TEXT("HUD Timer Updated: %s"), *TimerString); // 로그 레벨 조정
	}
}