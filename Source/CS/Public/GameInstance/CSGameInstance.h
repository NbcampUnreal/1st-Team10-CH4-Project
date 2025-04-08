#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CSTypes/CSGameTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CSGameInstance.generated.h"

UCLASS()
class CS_API UCSGameInstance : public UGameInstance
{
	GENERATED_BODY()
    
public:
    UCSGameInstance() : ExpectedPlayerCount(0), MatchType(EMatchType::EMT_None) {}

    virtual void Init() override;

    /** 세션 관련 함수 */
    UFUNCTION(BlueprintCallable)
    void HostSession(FName SessionName);

    UFUNCTION(BlueprintCallable)
    void FindSessions();

    void JoinFoundSession(const FOnlineSessionSearchResult& SearchResult);

    UFUNCTION(BlueprintCallable)
    void StartingSinglePlay();

    /** MatchType 관리 함수 */
    UFUNCTION(BlueprintCallable)
    void SetMatchType(EMatchType NewType) { MatchType = NewType; }

    UFUNCTION(BlueprintCallable)
    EMatchType GetMatchType() const { return MatchType; }

    UFUNCTION(BlueprintCallable)
    void ResetLobbySettings();

private:
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

    UPROPERTY()
    EMatchType MatchType;

    /** 세선 제어 객체 */
    IOnlineSessionPtr SessionInterface;
    TSharedPtr<FOnlineSessionSearch> SessionSearch;

public:
    UPROPERTY(BlueprintReadWrite, Category = "Match")
    int32 ExpectedPlayerCount;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables")
    UDataTable* CharacterData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables")
    UDataTable* AIData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables")
    UDataTable* LevelData;
};
