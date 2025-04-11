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

    // BlueprintReadOnly 추가! + VisibleAnywhere/EditAnywhere 등 필요시 추가
    UPROPERTY(ReplicatedUsing = OnRep_IsReady, BlueprintReadOnly, Category = "Player State")
    bool bIsReady;

    // BlueprintReadOnly 추가!
    UPROPERTY(ReplicatedUsing = OnRep_TeamID, BlueprintReadOnly, Category = "Player State")
    int32 TeamID;

    // BlueprintReadOnly 추가!
    UPROPERTY(ReplicatedUsing = OnRep_SelectedJob, BlueprintReadOnly, Category = "Player State")
    EJobTypes SelectedJob;
    
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player State")
    int32 PlayerIndex;
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player State")
    bool bIsAlive;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    bool ISLocalPlayerState() const;
};
