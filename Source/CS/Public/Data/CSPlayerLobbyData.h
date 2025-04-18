#pragma once

#include "CoreMinimal.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSPlayerLobbyData.generated.h"

USTRUCT(BlueprintType)
struct FPlayerLobbyData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EJobTypes SelectedJob;

	UPROPERTY(BlueprintReadWrite)
	int32 TeamID;

	UPROPERTY(BlueprintReadWrite)
	int32 PlayerIndex;
};