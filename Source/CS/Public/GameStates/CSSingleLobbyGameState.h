// CSSingleLobbyGameState.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h" // 베이스 클래스 변경 확인 (AGameState)
#include "CSSingleLobbyGameState.generated.h"

class APawn;

UCLASS()
class CS_API ACSSingleLobbyGameState : public AGameState // AGameState 상속 확인
{
	GENERATED_BODY()

public:
	ACSSingleLobbyGameState();

};