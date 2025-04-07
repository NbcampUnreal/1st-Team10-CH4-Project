#include "GameStates/CSStageGameState.h"
#include "Net/UnrealNetwork.h"
#include "Controller/CSPlayerController.h" // UI 업데이트 위해 필요 시
#include "Kismet/GameplayStatics.h"     // UI 업데이트 위해 필요 시

ACSStageGameState::ACSStageGameState()
{
    RemainingAICount = 0;
    TotalAICount = 0;
}

void ACSStageGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACSStageGameState, RemainingAICount);
    DOREPLIFETIME(ACSStageGameState, TotalAICount);
}

void ACSStageGameState::SetAIInfo(int32 Total)
{
    if (HasAuthority())
    {
        TotalAICount = Total;
        RemainingAICount = Total;
        // 강제로 OnRep 호출 (TotalAICount는 OnRep 없으므로 필요시)
        // OnRep_RemainingAICount(); // 서버에서도 초기 UI 업데이트 필요 시
    }
}

void ACSStageGameState::DecrementRemainingAICount()
{
    if (HasAuthority())
    {
        RemainingAICount = FMath::Max(0, RemainingAICount - 1);
        // OnRep 자동 호출됨
    }
}

void ACSStageGameState::OnRep_RemainingAICount()
{
    UE_LOG(LogTemp, Log, TEXT("[StageGS] 클라이언트: 남은 AI 수 업데이트됨: %d / %d"), RemainingAICount, TotalAICount);
    // 클라이언트의 PlayerController를 통해 UI 업데이트
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (ACSPlayerController* CS_PC = Cast<ACSPlayerController>(PC))
        {
            // CS_PC->UpdateRemainingAIUI(RemainingAICount, TotalAICount); // PlayerController에 함수 추가 필요
        }
    }
    // 델리게이트 방식 사용 시
    // OnRemainingAIUpdatedDelegate.Broadcast(RemainingAICount);
}