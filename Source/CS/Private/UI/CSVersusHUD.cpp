// CSVersusHUD.cpp
#include "UI/CSVersusHUD.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"        // GetGameState, GetPlayerController
#include "GameFramework/PlayerController.h" // HasAuthority
#include "GameStates/CSVersusGameState.h"   // VersusGameState 사용

// NativeConstruct 에서 초기화 (필요 시)
// void UCSVersusHUD::NativeConstruct()
// {
//     Super::NativeConstruct();
//     HostScoreUpdateInterval = 0.5f; // 0.5초마다
//     TimeSinceLastHostScoreUpdate = HostScoreUpdateInterval; // 처음에 바로 업데이트
//     CachedVersusGameState = GetWorld() ? Cast<ACSVersusGameState>(GetWorld()->GetGameState()) : nullptr;
//     // 초기 점수 표시
//     if(CachedVersusGameState.IsValid()) {
//          UpdateTeamScores(CachedVersusGameState->GetTeam0Score(), CachedVersusGameState->GetTeam1Score()); // GetTeamScore 함수 필요
//     } else {
//          UpdateTeamScores(0, 0);
//     }
//     if (ResultMessageText) ResultMessageText->SetVisibility(ESlateVisibility::Collapsed);
// }

void UCSVersusHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime); // 부모 Tick 호출

    // 호스트(리슨 서버)인 경우에만 직접 점수 업데이트
    APlayerController* PC = GetOwningPlayer();
    if (PC && PC->HasAuthority())
    {
        TimeSinceLastHostScoreUpdate += InDeltaTime;
        if (TimeSinceLastHostScoreUpdate >= HostScoreUpdateInterval)
        {
            TimeSinceLastHostScoreUpdate = 0.0f;

            // GameState 가져오기 (캐싱 활용)
            if (!CachedVersusGameState.IsValid()) {
                CachedVersusGameState = GetWorld() ? Cast<ACSVersusGameState>(GetWorld()->GetGameState()) : nullptr;
            }

            if (CachedVersusGameState.IsValid()) {
                // GameState에 GetTeam0Score(), GetTeam1Score() 함수가 있다고 가정
                // int32 Team0 = CachedVersusGameState->GetTeam0Score();
                // int32 Team1 = CachedVersusGameState->GetTeam1Score();
                // UpdateTeamScores(Team0, Team1); // BP 이벤트 호출
                // UE_LOG(LogTemp, Verbose, TEXT("Host HUD Tick: Updated Scores = %d / %d"), Team0, Team1);
            }
        }
    }
}