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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables")
    UDataTable* CharacterData;

    UPROPERTY(BlueprintReadWrite, Category = "Match")
    EMatchType MatchType;

    UPROPERTY(BlueprintReadWrite, Category = "Match")
    int32 ExpectedPlayerCount;

    UFUNCTION(BlueprintCallable)
    void ResetLobbySettings();

    UFUNCTION(BlueprintCallable)
    void SetMatchType(EMatchType NewType) { MatchType = NewType; }

};
