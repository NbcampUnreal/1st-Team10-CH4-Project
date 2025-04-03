#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CSLobbyGameState.generated.h"

UCLASS()
class CS_API ACSLobbyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(ReplicatedUsing = OnRep_SelectedMap)
	FName SelectedMap;

	UFUNCTION()
	void OnRep_SelectedMap();

	bool AreAllPlayerReady() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
