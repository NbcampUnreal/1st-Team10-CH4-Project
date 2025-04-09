#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Data/CSCharacterRow.h"
#include "CSTypes/CSCharacterTypes.h"
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
    UFUNCTION(BlueprintCallable)
    void ResetLobbySettings();

    UFUNCTION()
    void OnRep_IsReady();
    UFUNCTION()
    void OnRep_TeamID();
    UFUNCTION()
    void OnRep_SelectedJob();

    UPROPERTY(ReplicatedUsing = OnRep_IsReady)
    bool bIsReady;
    UPROPERTY(ReplicatedUsing = OnRep_TeamID)
    int32 TeamID;
    UPROPERTY(ReplicatedUsing = OnRep_SelectedJob)
    EJobTypes SelectedJob;
    
    UPROPERTY(Replicated)
    int32 PlayerIndex;
    UPROPERTY(Replicated)
    bool bIsAlive;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    bool ISLocalPlayerState() const;
};
