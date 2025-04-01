#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CSTypes/CSGameTypes.h"
#include "CSGameInstance.generated.h"

UCLASS()
class CS_API UCSGameInstance : public UGameInstance
{
	GENERATED_BODY()
    
public:
    UPROPERTY(BlueprintReadWrite, Category = "Match")
    EMatchType MatchType;

    UPROPERTY(BlueprintReadWrite, Category = "Map")
    FName SelectedMapName;

    UFUNCTION(BlueprintCallable)
    void SetMatchType(EMatchType NewType) { MatchType = NewType; }

    UFUNCTION(BlueprintCallable)
    void SetSelectedMapName(FName MapName) { SelectedMapName = MapName; }
};
