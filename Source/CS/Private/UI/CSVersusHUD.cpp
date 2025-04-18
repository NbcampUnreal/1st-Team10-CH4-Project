#include "UI/CSVersusHUD.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameStates/CSVersusGameState.h"


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
    Super::NativeTick(MyGeometry, InDeltaTime);

    APlayerController* PC = GetOwningPlayer();
    if (PC && PC->HasAuthority())
    {
        TimeSinceLastHostScoreUpdate += InDeltaTime;
        if (TimeSinceLastHostScoreUpdate >= HostScoreUpdateInterval)
        {
            TimeSinceLastHostScoreUpdate = 0.0f;

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