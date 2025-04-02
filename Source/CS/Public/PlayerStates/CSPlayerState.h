#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CSPlayerState.generated.h"

UCLASS()
class CS_API ACSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ACSPlayerState();

    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable)
    void SetIsReady(bool bNewReady);

    UFUNCTION()
    void OnRep_IsReady();
    UFUNCTION()
    void OnRep_TeamID();

    UPROPERTY(ReplicatedUsing = OnRep_IsReady)
    bool bIsReady;
    UPROPERTY(ReplicatedUsing = OnRep_TeamID)
    int32 TeamID;
    UPROPERTY(Replicated)
    int32 PlayerIndex;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
