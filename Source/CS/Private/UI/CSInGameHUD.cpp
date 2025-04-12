#include "UI/CSInGameHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Internationalization/Text.h"
#include "Kismet/GameplayStatics.h"        // GetGameState, GetPlayerController
#include "GameFramework/PlayerController.h" // HasAuthority
#include "GameFramework/GameStateBase.h"    // GetGameState (Base)
#include "GameStates/CSGameStateBase.h"   // ACSGameStateBase 캐스팅 위해

void UCSInGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
	UpdateHealth_Implementation(100.f, 100.f);
	UpdateMatchTimer_Implementation(0);

	// 호스트 타이머 업데이트 설정
	HostTimerUpdateInterval = 0.5f; // 예: 0.5초마다 업데이트
	TimeSinceLastHostTimerUpdate = HostTimerUpdateInterval; // 처음에 바로 업데이트하도록

	// GameState 캐싱 시도 (선택적)
	CachedGameState = GetWorld() ? Cast<ACSGameStateBase>(GetWorld()->GetGameState()) : nullptr;
}

// NativeTick 함수 구현 추가!
void UCSInGameHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 호스트(리슨 서버)인 경우에만 실행
	APlayerController* PC = GetOwningPlayer(); // GetOwningPlayer() 사용
	if (PC && PC->HasAuthority())
	{
		TimeSinceLastHostTimerUpdate += InDeltaTime;
		// 설정된 간격마다 업데이트
		if (TimeSinceLastHostTimerUpdate >= HostTimerUpdateInterval)
		{
			TimeSinceLastHostTimerUpdate = 0.0f; // 타이머 리셋

			// GameState 가져오기 (캐싱된 것 사용 또는 새로 가져오기)
			if (!CachedGameState.IsValid()) {
				CachedGameState = GetWorld() ? Cast<ACSGameStateBase>(GetWorld()->GetGameState()) : nullptr;
			}

			if (CachedGameState.IsValid()) {
				int32 CurrentRemainingTime = CachedGameState->GetRemainingMatchTime();
				// UpdateMatchTimer 함수 호출하여 UI 갱신
				UpdateMatchTimer(CurrentRemainingTime); // 내부적으로 _Implementation 호출됨
				// UE_LOG(LogTemp, Verbose, TEXT("Host HUD Tick: Updated Timer = %d"), CurrentRemainingTime);
			}
			// else { UE_LOG(LogTemp, Warning, TEXT("Host HUD Tick: GameState is invalid!")); }
		}
	}
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