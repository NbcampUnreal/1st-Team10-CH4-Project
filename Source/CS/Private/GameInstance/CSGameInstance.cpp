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
#include "Engine/NetDriver.h" // GetNetMode() 사용 위해

// 생성자 수정
UCSGameInstance::UCSGameInstance() :
	SessionInterface(nullptr),
	SessionSearch(nullptr),
	// PendingMatchType 제거
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
			// 세션 파괴 완료 콜백 등록 추가
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UCSGameInstance::OnDestroySessionComplete);
		}
		else { UE_LOG(LogTemp, Error, TEXT("Session Interface is invalid!")); }
	}
	else { UE_LOG(LogTemp, Error, TEXT("Online Subsystem not found!")); }
}

// Shutdown 함수 구현 추가
void UCSGameInstance::Shutdown()
{
	// 게임 종료 시 세션 파괴 시도
	if (SessionInterface.IsValid())
	{
		// 현재 활성화된 세션이 있는지 확인
		auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
		if (ExistingSession != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("GameInstance Shutdown: Destroying existing session: %s"), *ExistingSession->SessionName.ToString());
			// 세션 파괴 완료 콜백 등록 (선택적이지만 로그 확인용)
			DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
				FOnDestroySessionCompleteDelegate::CreateUObject(this, &UCSGameInstance::OnDestroySessionComplete)
			);
			if (!SessionInterface->DestroySession(NAME_GameSession))
			{
				UE_LOG(LogTemp, Warning, TEXT("GameInstance Shutdown: Failed to start DestroySession call. Clearing delegate handle."));
				SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
			}
		}
	}
	Super::Shutdown(); // 부모 클래스 Shutdown 호출 필수
}

// 세션 파괴 완료 콜백 구현 추가
void UCSGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance - OnDestroySessionComplete: Session=%s, Success=%d"), *SessionName.ToString(), bWasSuccessful);
	if (SessionInterface.IsValid())
	{
		// 콜백 핸들 정리
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}
	// 만약 HostSession 전에 파괴를 호출했다면, 여기서 CreateSession 재시도 가능
	// if (bWasSuccessful && bShouldCreateAfterDestroy) { HostSession(...); }
}

void UCSGameInstance::SetMatchType(EMatchType NewType)
{
	MatchType = NewType;
	UE_LOG(LogTemp, Log, TEXT("GameInstance MatchType set to: %d"), (int32)MatchType);
}

// HostSession 수정: MatchType 파라미터 받고 설정에 추가
void UCSGameInstance::HostSession(EMatchType TypeToHost)
{
	if (!SessionInterface.IsValid()) { /* ... */ return; }

	// 기존 세션 확인
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("HostSession: Found existing session. Destroying it first..."));
		// 세션 파괴 완료 시 다시 HostSession 을 호출하도록 플래그 설정 또는 람다 사용 등 필요
		// 여기서는 간단히 파괴만 호출하고 리턴 (비동기 처리)
		DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(
			// 파괴 성공 시 다시 HostSession 시도 (주의: 무한 루프 가능성 고려)
			FOnDestroySessionCompleteDelegate::CreateWeakLambda(this, [this, TypeToHost](FName SessionName, bool bWasSuccessful) {
				if (SessionInterface.IsValid()) SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
				if (bWasSuccessful) {
					UE_LOG(LogTemp, Log, TEXT("Previous session destroyed successfully. Retrying HostSession..."));
					// 약간의 딜레이 후 재시도 권장
					FTimerHandle RetryHandle;
					GetTimerManager().SetTimer(RetryHandle, [this, TypeToHost]() { HostSession(TypeToHost); }, 0.2f, false);
				}
				else {
					UE_LOG(LogTemp, Error, TEXT("Failed to destroy existing session. Cannot host."));
					ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
					if (PC) { PC->Client_ShowNoSessionPopup(); }
				}
				})
		);
		if (!SessionInterface->DestroySession(NAME_GameSession))
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to start DestroySession call in HostSession."));
			if (SessionInterface.IsValid()) SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
			// 파괴 호출 실패 시 즉시 팝업 알림
			ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if (PC) { PC->Client_ShowNoSessionPopup(); }
		}
		return; // 파괴 콜백 기다림
	}

	// --- 기존 세션 없을 경우 바로 생성 시도 ---
	SetMatchType(TypeToHost);
	// PendingMatchType = EMatchType::EMT_None; // 이 변수 제거됨

	FOnlineSessionSettings SessionSettings;
	// ... (세션 설정은 이전과 동일, MatchType 제거됨) ...
	SessionSettings.bIsLANMatch = true;
	SessionSettings.NumPublicConnections = (TypeToHost == EMatchType::EMT_Versus) ? 2 : 4;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bAllowJoinInProgress = false;
	SessionSettings.bUsesPresence = false;
	// SessionSettings.Set(FName("MATCH_TYPE"), ...); // 제거됨

	UE_LOG(LogTemp, Log, TEXT("Attempting to host session (MatchType %d stored locally)..."), (int32)TypeToHost);
	if (!SessionInterface->CreateSession(0, NAME_GameSession, SessionSettings)) { /* ... 실패 처리 ... */ }
	else { UE_LOG(LogTemp, Log, TEXT("CreateSession call initiated via GameInstance.")); }
}

// 세션 생성 완료 콜백
void UCSGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance - OnCreateSessionComplete: Success=%d"), bWasSuccessful);
	if (bWasSuccessful) {
		UE_LOG(LogTemp, Log, TEXT("Session created successfully! Opening LobbyLevel as listen server..."));
		UWorld* World = GetWorld();
		if (World) World->ServerTravel(TEXT("/Game/Blueprints/Hud/Maps/LobbyLevel?listen")); // 경로 확인
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("[GameInstance] Failed to create session: %s"), *SessionName.ToString());
		ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC) { PC->Client_ShowNoSessionPopup(); }
	}
}

// FindOrCreateSession 제거

// FindSessions 수정: MatchType 필터링 제거
void UCSGameInstance::FindSessions()
{
	UE_LOG(LogTemp, Log, TEXT("Finding sessions..."));
	if (!SessionInterface.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 20;
	SessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), false, EOnlineComparisonOp::Equals);
	// MatchType 필터링 제거!
	// SessionSearch->QuerySettings.Set(FName("MATCH_TYPE"), ..., EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	UE_LOG(LogTemp, Log, TEXT("FindSessions call initiated."));
}

// 세션 찾기 완료 콜백 수정
void UCSGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Log, TEXT("GameInstance - OnFindSessionsComplete: Success=%d"), bWasSuccessful);

	bool bFoundSession = false;
	if (bWasSuccessful && SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0) {
		// 찾은 첫 번째 유효한 세션에 참가 시도
		for (const FOnlineSessionSearchResult& Result : SessionSearch->SearchResults) {
			if (Result.IsValid()) {
				UE_LOG(LogTemp, Log, TEXT("Found valid session! Joining..."));
				JoinFoundSession(Result); // MatchType은 JoinFoundSession에서 설정
				bFoundSession = true;
				break;
			}
		}
	}

	// 유효한 세션을 찾지 못했거나 검색 실패 시
	if (!bFoundSession) {
		UE_LOG(LogTemp, Warning, TEXT("No valid session found or search failed."));
		// 호스팅 시도 대신 "세션 없음" 팝업 호출!
		ACSPlayerController* PC = Cast<ACSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (PC) { PC->Client_ShowNoSessionPopup(); }
	}
}

// JoinFoundSession 수정: SetMatchType 추가
void UCSGameInstance::JoinFoundSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!SessionInterface.IsValid()) return;
	UE_LOG(LogTemp, Log, TEXT("Attempting to join found session..."));
    // 중요: 메인 메뉴에서 클릭 시 설정된 MatchType을 사용해야 함
    // PendingMatchType 변수가 제거되었으므로, 이 함수를 호출하기 전에
    // GameInstance의 MatchType 멤버 변수가 올바르게 설정되어 있어야 함.
    // (메인 메뉴 버튼 클릭 핸들러에서 SetMatchType 호출)
    // SetMatchType(CorrectLobbyType); // CorrectLobbyType은 어떻게? -> Join 시점에는 알 수 없음.
    // => 로비 진입 후 GameState에서 MatchType 받아와야 함. Join 시점에서는 로컬 GI 설정 불필요.

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
	UE_LOG(LogTemp, Log, TEXT("Starting Arcade Mode (SingleModeLevel)."));
	// 경로 수정: "/Game/Blueprints/Hud/Maps/SingleModeLevel"
	UGameplayStatics::OpenLevel(World, FName("/Game/Blueprints/Hud/Maps/SingleModeLevel")); // 전체 경로 사용
}

// ServerTravelToLevel 수정: 경로 및 로그 개선
void UCSGameInstance::ServerTravelToLevel(FName LevelID)
{
	UWorld* World = GetWorld();
	if (!LevelData || !World) return;
	const FString ContextStr = TEXT("ServerTravelToLevel");
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, ContextStr)) {
		if (!Row->MapPath.IsEmpty()) {
			// MapPath 가 "/Game/Blueprints/Hud/Maps/LevelName" 형태라고 가정하고 ?listen 추가
			FString TravelURL = Row->MapPath + TEXT("?listen");
			UE_LOG(LogTemp, Log, TEXT("Server traveling to level from LevelData: %s"), *TravelURL);
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
			// MapPath 가 "/Game/Blueprints/Hud/Maps/LevelName" 형태라고 가정
			UE_LOG(LogTemp, Log, TEXT("Opening level by ID from LevelData: %s (Path: %s)"), *LevelID.ToString(), *Row->MapPath);
			UGameplayStatics::OpenLevel(World, FName(*Row->MapPath)); // 경로 직접 사용
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

const FLevelRow* UCSGameInstance::FindLevelRow(FName RowName) const
{
	if (!LevelData) return nullptr;
	const FString Context = TEXT("FineLevelRow");

	return LevelData->FindRow<FLevelRow>(RowName, Context);
}
