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
    UCSGameInstance() : MatchType(EMatchType::EMT_None), SelectedMap(NAME_None) {}

    UPROPERTY(BlueprintReadWrite, Category = "Match")
    EMatchType MatchType;

    UPROPERTY(BlueprintReadWrite, Category = "Map")
    FName SelectedMap;

    UFUNCTION(BlueprintCallable)
    void SetMatchType(EMatchType NewType) { MatchType = NewType; }

    UFUNCTION(BlueprintCallable)
    void SetSelectedMapName(FName MapName) { SelectedMap = MapName; }
};
