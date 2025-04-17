// CSGameStateBase.cpp

#include "GameStates/CSGameStateBase.h"
#include "Controller/CSPlayerController.h" // PlayerController 헤더 포함
#include "UI/CSUIBaseWidget.h"         // UI 베이스 위젯 헤더 포함
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACSGameStateBase::ACSGameStateBase()
{
    MatchPhase = EMatchPhase::EMP_None;
    RemainingMatchTime = 0;
}

void ACSGameStateBase::OnRep_MatchPhase()
{
    // 이 함수는 클라이언트에서만 호출됨
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->IsLocalController()) {
        ACSPlayerController* CSPC = Cast<ACSPlayerController>(PC);
        if (CSPC) {
            UE_LOG(LogTemp, Log, TEXT("GameState OnRep_MatchPhase: New Phase = %d (Client)"), (int32)MatchPhase);
            UCSUIBaseWidget* CurrentUI = CSPC->GetCurrentUI();
            if (CurrentUI) {
                CurrentUI->HandleMatchPhaseChanged(MatchPhase);
                UE_LOG(LogTemp, Log, TEXT("GameState OnRep_MatchPhase: Called HandleMatchPhaseChanged on Client UI."));
            }
            else { UE_LOG(LogTemp, Warning, TEXT("GameState OnRep_MatchPhase: CurrentUI is NULL on Client.")); }
        }
    }
}

void ACSGameStateBase::OnRep_RemainingMatchTime()
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->IsLocalController()) {
        ACSPlayerController* CSPC = Cast<ACSPlayerController>(PC);
        if (CSPC) {
            UCSUIBaseWidget* CurrentUI = CSPC->GetCurrentUI();
            if (CurrentUI) {
                CurrentUI->UpdateMatchTimer(RemainingMatchTime);
            }
            else { UE_LOG(LogTemp, Warning, TEXT("GameState OnRep_RemainingMatchTime: CurrentUI is NULL on Client.")); }
        }
    }
}

void ACSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // 조건부 리플리케이션 등을 사용하지 않는다면 아래와 같이 단순화 가능
    DOREPLIFETIME_CONDITION_NOTIFY(ACSGameStateBase, MatchPhase, COND_None, REPNOTIFY_Always); // Notify 항상 발생
    DOREPLIFETIME_CONDITION_NOTIFY(ACSGameStateBase, RemainingMatchTime, COND_None, REPNOTIFY_Always);
    // DOREPLIFETIME(ACSGameStateBase, MatchPhase); // REPNOTIFY_OnChanged 가 기본값
    // DOREPLIFETIME(ACSGameStateBase, RemainingMatchTime);
    DOREPLIFETIME(ACSGameStateBase, MatchResult); // MatchResult 도 리플리케이트
}