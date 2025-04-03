#include "GameModes/CSSingleLobbyGameMode.h"
#include "GameStates/CSSingleLobbyGameState.h"
#include "Controller/CSPlayerController.h"     // 기본 PlayerController 클래스 설정용
#include "GameInstance/CSGameInstance.h"     // 캐릭터 선택 정보 저장/로드용 (주석처리됨)
// #include "PlayerStates/CSPlayerState.h"    // 캐릭터 선택 정보 저장/로드용 (주석처리됨)
#include "Kismet/GameplayStatics.h"

ACSSingleLobbyGameMode::ACSSingleLobbyGameMode()
{
	// 로비에서는 플레이어가 직접 조종할 Pawn이 필요 없을 수 있으니 null이나 SpectatorPawn으로 설정.
	DefaultPawnClass = nullptr; // 또는 SpectatorPawnClass
	PlayerControllerClass = ACSPlayerController::StaticClass(); // 사용할 플레이어 컨트롤러 클래스
	GameStateClass = ACSSingleLobbyGameState::StaticClass();   // 사용할 게임 상태 클래스
	// PlayerStateClass = ACSPlayerState::StaticClass(); // 커스텀 PlayerState 사용 시 주석 해제
}

void ACSSingleLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		UE_LOG(LogTemp, Log, TEXT("[LobbyGM] 플레이어 %s 로비에 들어옴."), *NewPlayer->GetName());

		// 로비 UI 표시는 PlayerController의 BeginPlay에서 처리하는 게 좋음.
		// 여기서 서버측 로직(예: 플레이어 목록 업데이트 알림 등)이 필요하면 추가.
	}
}

void ACSSingleLobbyGameMode::RequestStartTutorial(APlayerController* SelectingPlayer /*, TSubclassOf<APawn> SelectedCharacterClass*/)
{
	if (!SelectingPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyGM] RequestStartTutorial 호출했지만 PlayerController가 없음."));
		return;
	}

	// TODO: 어떤 캐릭터를 선택했는지 정보를 함수 인자(SelectedCharacterClass) 등으로 받아와야 함.

	UE_LOG(LogTemp, Log, TEXT("[LobbyGM] 플레이어 %s 가 튜토리얼 시작 요청함 (선택 캐릭터 정보 저장 필요). 다음 레벨: %s"),
		*SelectingPlayer->GetName(),
		*TutorialLevelName.ToString());

	// --- 캐릭터 선택 정보 저장 로직 (현재는 주석 처리) ---
	// 이 부분은 GameInstance 또는 PlayerState에 선택된 캐릭터 정보를 저장해야 함.
	// CSTutorialGameMode가 시작될 때 이 정보를 읽어서 사용함.

	// 방법 1: GameInstance 사용 예시 (GameInstance에 변수 정의 필요)
	/*
	UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
	if (GI)
	{
		// TODO: GI에 SelectedCharacterClass 같은 변수 정의하고 값 저장 필요.
		// GI->SelectedCharacterClass = SelectedCharacterClass; // 함수 인자로 받은 클래스 정보 저장
		UE_LOG(LogTemp, Log, TEXT("선택 캐릭터 정보 GameInstance에 저장함 (가정)."));
	}
	*/

	// 방법 2: PlayerState 사용 예시 (PlayerState에 변수 정의 및 리플리케이션 필요)
	/*
	ACSPlayerState* PS = SelectingPlayer->GetPlayerState<ACSPlayerState>(); // ACSPlayerState로 캐스팅 필요
	if (PS)
	{
		// TODO: PS에 SelectedCharacterClass 같은 변수 정의 및 Replicate 설정 필요.
		// PS->SetSelectedCharacter(SelectedCharacterClass); // PlayerState에 값 설정 (서버에서)
		UE_LOG(LogTemp, Log, TEXT("선택 캐릭터 정보 PlayerState에 저장함 (가정)."));
	}
	*/
	// --- 캐릭터 선택 정보 저장 로직 끝 ---

	// 튜토리얼 레벨로 이동
	UGameplayStatics::OpenLevel(this, TutorialLevelName);

	// 참고: Seamless Travel 사용 시: World->ServerTravel(TutorialLevelName.ToString());
}