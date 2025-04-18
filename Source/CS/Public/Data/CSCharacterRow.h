#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSCharacterRow.generated.h"

USTRUCT(BlueprintType)
struct FCharacterRow : public FTableRowBase
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<APawn> CharacterClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UTexture2D> Portrait;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;
};
