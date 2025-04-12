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
    // 로컬 플레이어 컨트롤러 가져오기 (싱글/멀티 공통)
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->IsLocalController())
    {
        ACSPlayerController* CSPC = Cast<ACSPlayerController>(PC);
        if (CSPC)
        {
            UE_LOG(LogTemp, Log, TEXT("GameState OnRep_MatchPhase: New Phase = %d"), (int32)MatchPhase);
            // PlayerController의 함수를 호출하여 상태 변경 알림 (선택적)
            // CSPC->OnMatchPhaseChanged(MatchPhase);

            // 또는 PlayerController를 통해 현재 UI 위젯을 직접 가져와서 처리
            UCSUIBaseWidget* CurrentUI = CSPC->GetCurrentUI();
            if (CurrentUI)
            {
                // 위젯의 HandleMatchPhaseChanged 함수 호출 (BlueprintNativeEvent)
                CurrentUI->HandleMatchPhaseChanged(MatchPhase);
                UE_LOG(LogTemp, Log, TEXT("GameState OnRep_MatchPhase: Called HandleMatchPhaseChanged on Current UI."));

                // --- 여기서 입력 모드 변경을 추가할 수도 있음 ---
                if (MatchPhase == EMatchPhase::EMP_Playing)
                {
                    UE_LOG(LogTemp, Warning, TEXT("GameState OnRep_MatchPhase: Setting Input Mode to GameOnly (via GameState RepNotify)"));
                    FInputModeGameOnly InputModeData;
                    CSPC->SetInputMode(InputModeData);
                    CSPC->bShowMouseCursor = false;
                    // 포커스 설정도 추가 가능
                    FSlateApplication::Get().SetUserFocusToGameViewport(0);
                }
                else // Waiting, Finished, None 등
                {
                    UE_LOG(LogTemp, Warning, TEXT("GameState OnRep_MatchPhase: Setting Input Mode to UI Only (via GameState RepNotify)"));
                    FInputModeUIOnly InputModeData;
                    // InputModeData.SetWidgetToFocus(...); // 포커스 줄 위젯 필요 시 설정
                    CSPC->SetInputMode(InputModeData);
                    CSPC->bShowMouseCursor = true;
                }
                // -----------------------------------------------
            }
            else { UE_LOG(LogTemp, Warning, TEXT("GameState OnRep_MatchPhase: CurrentUI is NULL.")); }
        }
    }
}

void ACSGameStateBase::OnRep_RemainingMatchTime()
{
    // 로컬 플레이어 컨트롤러 가져오기
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && PC->IsLocalController())
    {
        ACSPlayerController* CSPC = Cast<ACSPlayerController>(PC);
        if (CSPC)
        {
            UCSUIBaseWidget* CurrentUI = CSPC->GetCurrentUI();
            if (CurrentUI)
            {
                // 위젯의 UpdateMatchTimer 함수 호출 (BlueprintNativeEvent)
                CurrentUI->UpdateMatchTimer(RemainingMatchTime);
                // UE_LOG(LogTemp, Verbose, TEXT("GameState OnRep_RemainingMatchTime: Called UpdateMatchTimer on Current UI. Time = %d"), RemainingMatchTime);
            }
            else { UE_LOG(LogTemp, Warning, TEXT("GameState OnRep_RemainingMatchTime: CurrentUI is NULL.")); }
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