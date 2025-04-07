#pragma once

#include "CoreMinimal.h"
#include "GameStates/CSGameStateBase.h"
#include "CSStageGameState.generated.h"

UCLASS()
class CS_API ACSStageGameState : public ACSGameStateBase
{
    GENERATED_BODY()

public:
    ACSStageGameState();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // 서버에서 호출하여 AI 총 수 및 남은 수 설정
    void SetAIInfo(int32 Total);
    void DecrementRemainingAICount();

    // 클라이언트에서 남은 AI 수를 가져오기 위함
    UFUNCTION(BlueprintPure, Category = "Game State")
    int32 GetRemainingAICount() const { return RemainingAICount; }

    UFUNCTION(BlueprintPure, Category = "Game State")
    int32 GetTotalAICount() const { return TotalAICount; }

protected:
    UFUNCTION()
    void OnRep_RemainingAICount();

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_RemainingAICount, Category = "AI")
    int32 RemainingAICount;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Replicated, Category = "AI")
    int32 TotalAICount;

    // 클라이언트 UI 업데이트를 위한 델리게이트 (선택적)
    // DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingAIUpdated, int32, NewCount);
    // UPROPERTY(BlueprintAssignable, Category = "Events")
    // FOnRemainingAIUpdated OnRemainingAIUpdatedDelegate;
};