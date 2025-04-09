#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CSTypes/CSGameTypes.h"
#include "CSTypes/CSCharacterTypes.h"
#include "Data/CSCharacterRow.h"
#include "Data/CSLevelRow.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"      // 필수
#include "FindSessionsCallbackProxy.h"  // FOnlineSessionSearchResult 사용 위해 추가
#include "CSGameInstance.generated.h"

UCLASS()
class CS_API UCSGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UCSGameInstance(); // 생성자에서 초기화 순서 맞춤
	virtual void Init() override;

	/** 세션 관련 함수 */
	UFUNCTION(BlueprintCallable, Category = "Session")
	void HostSession(EMatchType TypeToHost); // MatchType 파라미터 추가

	// FindOrCreateSession 추가
	UFUNCTION(BlueprintCallable, Category = "Session")
	void FindOrCreateSession(EMatchType TypeToFindOrCreate);

	// JoinFoundSession 유지
	void JoinFoundSession(const FOnlineSessionSearchResult& SearchResult);

	/** 싱글 플레이 시작 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void StartArcadeMode();

	/** 레벨 이동 */
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void ServerTravelToLevel(FName LevelID);
	UFUNCTION(BlueprintCallable, Category = "Game Flow")
	void OpenLevelByID(FName LevelID);

	/** MatchType 관리 */
	UFUNCTION(BlueprintCallable)
	void SetMatchType(EMatchType NewType);
	UFUNCTION(BlueprintPure, Category = "Game Flow") // BlueprintPure 로 변경
	EMatchType GetMatchType() const { return MatchType; }
	UFUNCTION(BlueprintCallable)
	void ResetLobbySettings();

	/** 헬퍼 함수 */
	const FCharacterRow* FindCharacterRowByJob(EJobTypes Job) const;
	const FLevelRow* FindLevelRow(FName RowName) const;

private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void FindSessionsInternal(EMatchType TypeToFind); // 내부 검색 함수

	// 멤버 선언 순서 변경
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	EMatchType PendingMatchType; // 여기로 이동

	UPROPERTY()
	EMatchType MatchType;

public:
	UPROPERTY(BlueprintReadWrite, Category = "Match")
	int32 ExpectedPlayerCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables")
	TObjectPtr<UDataTable> CharacterData; // TObjectPtr 사용 권장

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables")
	TObjectPtr<UDataTable> AIData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DataTables")
	TObjectPtr<UDataTable> LevelData;
};
