#pragma once

#include "CoreMinimal.h"
#include "CSGameTypes.generated.h"

UENUM(BlueprintType)
enum class EMatchType : uint8
{
	EMT_Single UMETA(DisplayName = "SinglePlay"),
	EMT_Versus UMETA(DisplayName = "Versus"),
	EMT_Coop UMETA(DisplayName = "Coop")
};

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
	EMP_Waiting    UMETA(DisplayName = "Waiting"),
	EMP_Playing    UMETA(DisplayName = "Playing"),
	EMP_Finished   UMETA(DisplayName = "Finished")
};