#include "GameStates/CSSingleLobbyGameState.h"
// #include "Net/UnrealNetwork.h" // 리플리케이션 사용 시 필요 (현재 주석 처리)
// #include "GameFramework/Pawn.h"  // TSubclassOf<APawn> 사용 시 필요 (현재 주석 처리)

ACSSingleLobbyGameState::ACSSingleLobbyGameState()
{
	// 생성자에서 초기화할 내용이 있다면 추가.
}


// --- 선택 가능 캐릭터 목록 리플리케이션 설정 (예시, 현재 주석 처리) ---
/*
void ACSSingleLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// AvailableCharacterClasses 변수를 모든 클라이언트에게 리플리케이트함.
	DOREPLIFETIME(ACSSingleLobbyGameState, AvailableCharacterClasses);
}

void ACSSingleLobbyGameState::LoadAvailableCharacters()
{
	// TODO: 데이터 테이블, 에셋 레지스트리 등에서 실제 선택 가능한 캐릭터 클래스 목록을 읽어와서
	// AvailableCharacterClasses 배열을 채우는 로직 구현 필요.
	// 예시:
	// AvailableCharacterClasses.Empty();
	// AvailableCharacterClasses.Add(FindObject<UClass>(nullptr, TEXT("/Game/Blueprints/Characters/BP_WarriorCharacter.BP_WarriorCharacter_C")));
	// AvailableCharacterClasses.Add(FindObject<UClass>(nullptr, TEXT("/Game/Blueprints/Characters/BP_MageCharacter.BP_MageCharacter_C")));
	// ...
	UE_LOG(LogTemp, Log, TEXT("Available characters loaded (assumed)."));

	// 멀티플레이어 게임이라면, 변수가 변경되었음을 클라이언트에 알리는 과정이 필요할 수 있음 (OnRep 함수 등).
	// 싱글 플레이 로비에서는 서버에서 채우고 클라이언트가 접속 시점에 받거나, 필요 시점에 요청해서 받는 방식도 가능.
}
*/
// --- 선택 가능 캐릭터 목록 리플리케이션 설정 끝 ---