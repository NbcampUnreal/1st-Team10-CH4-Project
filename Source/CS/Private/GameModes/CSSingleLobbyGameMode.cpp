#include "GameModes/CSSingleLobbyGameMode.h"
#include "GameStates/CSSingleLobbyGameState.h" // 헤더 포함
#include "Controller/CSPlayerController.h"     // 기본 PlayerController 클래스 설정용
#include "GameInstance/CSGameInstance.h"     // 캐릭터 선택 정보 저장/로드용 (주석 유지)
// #include "PlayerStates/CSPlayerState.h"    // 캐릭터 선택 정보 저장/로드용 (주석 유지)
#include "Kismet/GameplayStatics.h"

ACSSingleLobbyGameMode::ACSSingleLobbyGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = ACSPlayerController::StaticClass();
		GameStateClass = ACSSingleLobbyGameState::StaticClass();
		// PlayerStateClass = ACSPlayerState::StaticClass();
}

void ACSSingleLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		UE_LOG(LogTemp, Log, TEXT("[LobbyGM] 플레이어 %s 로비에 들어옴."), *NewPlayer->GetName());
	}
}

void ACSSingleLobbyGameMode::RequestStartTutorial(APlayerController* SelectingPlayer /*, TSubclassOf<APawn> SelectedCharacterClass*/)
{
	if (!SelectingPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[LobbyGM] RequestStartTutorial 호출했지만 PlayerController가 없음."));
		return;
	}

	 // 주석 유지: 어떤 캐릭터를 선택했는지 PlayerController의 RPC 호출 등을 통해 받아오는 로직 필요.
		// TODO: 어떤 캐릭터를 선택했는지 정보를 함수 인자(SelectedCharacterClass) 등으로 받아와야 함.

		UE_LOG(LogTemp, Log, TEXT("[LobbyGM] 플레이어 %s 가 튜토리얼 시작 요청함 (선택 캐릭터 정보 저장 필요). 다음 레벨: %s"),
			*SelectingPlayer->GetName(),
			*TutorialLevelName.ToString());

	// --- 캐릭터 선택 정보 저장 로직 (주석 유지) ---
	 // 주석 유지: 아래 로직들은 CSGameInstance나 CSPlayerState에 'SelectedCharacterClass' 같은 변수가 실제로 정의되어야 사용 가능함.

		// 방법 1: GameInstance 사용 예시
		/*
		UCSGameInstance* GI = GetGameInstance<UCSGameInstance>();
		if (GI)
		{
			// TODO: GI에 SelectedCharacterClass 같은 변수 정의하고 값 저장 필요.
			// GI->SelectedCharacterClass = SelectedCharacterClass; // 함수 인자로 받은 클래스 정보 저장
			UE_LOG(LogTemp, Log, TEXT("선택 캐릭터 정보 GameInstance에 저장함 (가정)."));
		}
		*/

		// 방법 2: PlayerState 사용 예시
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
		// UGameplayStatics::OpenLevel(this, TutorialLevelName);
}