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
    UCSGameInstance() : MatchType(EMatchType::EMT_None), ExpectedPlayerCount(0) {}

    UFUNCTION(BlueprintCallable)
    void SetMatchType(EMatchType NewType) { MatchType = NewType; }

    UFUNCTION(BlueprintCallable)
    EMatchType GetMatchType() const { return MatchType; }

    UFUNCTION(BlueprintCallable)
    void ResetLobbySettings();

    UPROPERTY(BlueprintReadWrite, Category = "Match")
    int32 ExpectedPlayerCount;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables")
    UDataTable* CharacterData;
private:
    UPROPERTY()
    EMatchType MatchType;
};
