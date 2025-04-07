#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSAIRow.generated.h"

USTRUCT(BlueprintType)
struct FAIRow : public FTableRowBase
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<APawn> AICharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
};
