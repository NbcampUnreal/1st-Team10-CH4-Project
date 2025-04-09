#include "GameInstance/CSGameInstance.h"
#include "Controller/CSPlayerController.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"   // 세션 설정 및 상수 정의 헤더
#include "OnlineSubsystemUtils.h"    // 관련 유틸리티
#include "Data/CSLevelRow.h"
#include "Data/CSCharacterRow.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// 생성자 수정
UCSGameInstance::UCSGameInstance() :
	SessionInterface(nullptr),
	SessionSearch(nullptr),
	PendingMatchType(EMatchType::EMT_None),
	MatchType(EMatchType::EMT_MainMenu), // 기본값 MainMenu
	ExpectedPlayerCount(0),
	CharacterData(nullptr),
	AIData(nullptr),
	LevelData(nullptr)
{
}

void UCSGameInstance::Init()
{
	Super::Init();
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem) {
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCSGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCSGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCSGameInstance::OnJoinSessionComplete);
		}
		else { UE_LOG(LogTemp, Error, TEXT("Session Interface is invalid!")); }
	}
	else { UE_LOG(LogTemp, Error, TEXT("Online Subsystem not found!")); }
}

void UCSGameInstance::SetMatchType(EMatchType NewType)
{
	MatchType = NewType;
	UE_LOG(LogTemp, Log, TEXT("GameInstance MatchType set to: %d"), (int32)MatchType);
}

// HostSession 수정: MatchType 파라미터 받고 설정에 추가
void UCSGameInstance::HostSession(EMatchType TypeToHost)
{
	if (!SessionInterface.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("HostSession: Session Interface is invalid!"));
		// 실패 알림
		ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC) { PC->Client_ShowNoSessionPopup(); } // 실패 시 "세션 없음" 알림 재활용
		return;
	}

	// 기존 세션 확인 및 파괴 로직 (선택적)
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Destroying existing session before creating a new one..."));
		// TODO: Add DestroySession logic if needed and wait
		// SessionInterface->DestroySession(NAME_GameSession);
		// return; // 임시: 파괴 로직 없으면 바로 생성 시도
	}

	SetMatchType(TypeToHost); // GameInstance MatchType 설정
	PendingMatchType = EMatchType::EMT_None;

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = (TypeToHost == EMatchType::EMT_Versus) ? 2 : 4; // VS 2명, 나머진 4명
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = false;
	SessionSettings.bUsesPresence = false; // Presence 사용 안 함
	SessionSettings.Set(FName("MATCH_TYPE"), (int32)TypeToHost, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	UE_LOG(LogTemp, Log, TEXT("Attempting to host session with MatchType: %d"), (int32)TypeToHost);
	if (!SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings)) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to start CreateSession call in GameInstance."));
		// 실패 알림
		ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC) { PC->Client_ShowNoSessionPopup(); }
	}
	else { UE_LOG(LogTemp, Log, TEXT("CreateSession call initiated via GameInstance.")); }
}

// 세션 생성 완료 콜백
void UCSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance - OnCreateSessionComplete: Success=%d"), bWasSuccessful);
	// 델리게이트 핸들 정리는 필요 없음 (AddUObject 사용 시 자동 관리됨)
	if (bWasSuccessful) {
		UE_LOG(LogTemp, Log, TEXT("Session created successfully! Opening LobbyLevel as listen server..."));
		UWorld* World = GetWorld(); // GetWorld() 호출 확인
		if (World) World->ServerTravel(TEXT("/Game/Maps/LobbyLevel?listen")); // 경로 확인
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] Failed to create session: %s"), *SessionName.ToString());
		ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC) { PC->Client_ShowNoSessionPopup(); }
	}
}

// FindOrCreateSession 구현
void UCSGameInstance::FindOrCreateSession(EMatchType TypeToFindOrCreate)
{
	if (!SessionInterface.IsValid()) {
		UE_LOG(LogTemp, Error, TEXT("FindOrCreateSession: Session Interface is invalid!"));
		ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC) { PC->Client_ShowNoSessionPopup(); }
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("FindOrCreateSession called for MatchType: %d"), (int32)TypeToFindOrCreate);
	PendingMatchType = TypeToFindOrCreate; // 타입 저장
	FindSessionsInternal(TypeToFindOrCreate); // 검색 시작
}

// FindSessions 내부 로직 함수
void UCSGameInstance::FindSessionsInternal(EMatchType TypeToFind)
{
	UE_LOG(LogTemp, Log, TEXT("Finding sessions internally for MatchType: %d..."), (int32)TypeToFind);
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 20; // 결과 수 증가
	SessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), false, EOnlineComparisonOp::Equals);
	// MatchType 필터링
	SessionSearch->QuerySettings.Set(FName("MATCH_TYPE"), (int32)TypeToFind, EOnlineComparisonOp::Equals);

	// 검색 시작 (콜백은 Init에서 등록됨)
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	UE_LOG(LogTemp, Log, TEXT("FindSessionsInternal call initiated."));
}

// 세션 찾기 완료 콜백 수정
void UCSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance - OnFindSessionsComplete: Success=%d"), bWasSuccessful);

	if (bWasSuccessful && SessionSearch.IsValid()) {
		bool bFoundMatchingSession = false;
		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults) {
			if (Result.IsValid()) {
				int32 SessionMatchTypeInt;
				if (Result.Session.SessionSettings.Get(FName("MATCH_TYPE"), SessionMatchTypeInt)) {
					EMatchType SessionMatchType = (EMatchType)SessionMatchTypeInt;
					if (SessionMatchType == PendingMatchType) { // 타입 일치 확인
						UE_LOG(LogTemp, Log, TEXT("Found matching session! Joining..."));
						JoinFoundSession(Result);
						bFoundMatchingSession = true;
						break; // 첫 번째 맞는 세션에 참가
					}
				}
			}
		}

		if (!bFoundMatchingSession) {
			UE_LOG(LogTemp, Log, TEXT("No matching session found. Attempting to host..."));
			HostSession(PendingMatchType); // 맞는 세션 없으면 호스트 시도
		}
	}
	else {
		// 검색 자체가 실패
		UE_LOG(LogTemp, Warning, TEXT("Session search failed or SessionSearch is invalid. Attempting to host..."));
		HostSession(PendingMatchType); // 검색 실패 시 호스트 시도
	}
}

// JoinFoundSession 수정: SetMatchType 추가
void UCSGameInstance::JoinFoundSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!SessionInterface.IsValid()) return;
	UE_LOG(LogTemp, Log, TEXT("Attempting to join found session..."));
	SetMatchType(PendingMatchType); // 참가할 세션의 MatchType 설정
	SessionInterface->JoinSession(0, NAME_GameSession, SearchResult);
}

// OnJoinSessionComplete 수정: 로그 강화
void UCSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance - OnJoinSessionComplete: Result=%d"), (int32)Result);
	if (!SessionInterface.IsValid()) return;

	FString ConnectString;
	if (Result == EOnJoinSessionCompleteResult::Success && SessionInterface->GetResolvedConnectString(SessionName, ConnectString)) {
		UE_LOG(LogTemp, Log, TEXT("Join successful! Traveling to server: %s"), *ConnectString);
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0); // GetWorld() 사용 확인
		if (PlayerController) {
			PlayerController->ClientTravel(ConnectString, TRAVEL_Absolute);
		}
		else { UE_LOG(LogTemp, Error, TEXT("Failed to get PlayerController for ClientTravel!")); }
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] Failed to join session %s, Result: %d"), *SessionName.ToString(), (int32)Result);
		ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC) { PC->Client_ShowNoSessionPopup(); } // 실패 시 "세션 없음" 알림
	}
}

// StartArcadeMode 구현
void UCSGameInstance::StartArcadeMode()
{
	UWorld* World = GetWorld();
	if (!World) return;
	SetMatchType(EMatchType::EMT_Single);
	UE_LOG(LogTemp, Log, TEXT("Starting Arcade Mode (StageLevel)."));
	UGameplayStatics::OpenLevel(World, FName("StageLevel")); // 레벨 이름 확인
}

// ServerTravelToLevel 수정: 경로 및 로그 개선
void UCSGameInstance::ServerTravelToLevel(FName LevelID)
{
	UWorld* World = GetWorld();
	if (!LevelData || !World) return;

	const FString ContextStr = TEXT("ServerTravelToLevel");
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, ContextStr)) {
		if (!Row->MapPath.IsEmpty()) {
			// MapPath 가 "/Game/Maps/LevelName" 형태라고 가정
			FString TravelURL = Row->MapPath + TEXT("?listen");
			UE_LOG(LogTemp, Log, TEXT("Server traveling to level: %s"), *TravelURL);
			World->ServerTravel(TravelURL);
		}
		else { UE_LOG(LogTemp, Error, TEXT("ServerTravelToLevel: MapPath is empty for LevelID %s"), *LevelID.ToString()); }
	}
	else { UE_LOG(LogTemp, Error, TEXT("ServerTravelToLevel: LevelID %s not found in LevelData."), *LevelID.ToString()); }
}

// OpenLevelByID 수정: 경로 및 로그 개선
void UCSGameInstance::OpenLevelByID(FName LevelID)
{
	UWorld* World = GetWorld();
	if (!LevelData || !World) return;

	const FString ContextStr = TEXT("OpenLevelByID");
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, ContextStr)) {
		if (!Row->MapPath.IsEmpty()) {
			UE_LOG(LogTemp, Log, TEXT("Opening level by ID: %s (Path: %s)"), *LevelID.ToString(), *Row->MapPath);
			UGameplayStatics::OpenLevel(World, FName(*Row->MapPath));
		}
		else { UE_LOG(LogTemp, Error, TEXT("OpenLevelByID: MapPath is empty for LevelID %s"), *LevelID.ToString()); }
	}
	else { UE_LOG(LogTemp, Error, TEXT("OpenLevelByID: LevelID %s not found in LevelData."), *LevelID.ToString()); }
}

void UCSGameInstance::ResetLobbySettings() { ExpectedPlayerCount = 0; }

const FCharacterRow* UCSGameInstance::FindCharacterRowByJob(EJobTypes Job) const
{
	if (!CharacterData) return nullptr;
	const UEnum* EnumPtr = StaticEnum<EJobTypes>();
	if (!EnumPtr) return nullptr;
	// Enum 값에서 직접 FName 생성 시도 (Enum 이름과 Row 이름이 같다고 가정)
	FString EnumString = EnumPtr->GetNameByValue((int64)Job).ToString();
	// "EJT_" 접두사 제거 (데이터 테이블 행 이름에 접두사가 없다면)
	EnumString.RemoveFromStart(TEXT("EJT_"));
	FName RowName = FName(*EnumString);
	// FString RowNameString = EnumPtr->GetNameStringByValue((int64)Job); // 이 방식도 유효
	// FName RowName(*RowNameString);
	return CharacterData->FindRow<FCharacterRow>(RowName, TEXT("FindCharacterRowByJob"));
}
