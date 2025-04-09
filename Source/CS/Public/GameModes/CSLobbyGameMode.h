#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CSTypes/CSGameTypes.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSLobbyGameMode.generated.h"

UCLASS()
class CS_API ACSLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ACSLobbyGameMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	void StartMatchIfReady();

	UFUNCTION(BlueprintCallable)
	void TryStartMatch();

	UFUNCTION(BlueprintCallable)
	void ChangeTeam(APlayerController* Player);

	UFUNCTION(BlueprintCallable)
	void SetSelectedPlayerJob(APlayerController* Player, EJobTypes NewJob);

protected:
	/** 로비 캐릭터 컨트롤 하기 위한 Map */
	UPROPERTY()
	TMap<APlayerController*, class ACSLobbyCharacter*> LobbyCharacterMap;

	/** 현재 매치 타입 (Versus / Coop) */
	EMatchType MatchType;

	/** 초기 카메라 세팅 */
	void SetViewLobbyCam(APlayerController* NewPlayer);

	/** 팀 균형 확인 */
	bool IsTeamBalanced();

	/** 전체 로비 캐릭터 배치 */
	void PositionLobbyCharacters();

	/** Versus 모드 로직 */
	void PositionVersusCharacters();

	/** Coop 모드 로직 */
	void PositionCoopCharacters();

	/** 공통 유틸 */
	TMap<int32, TArray<ESpawnSlotType>> GetSlotPriorityForMatchType() const;
	void GetAllSpawnManagers(TArray<class ACSSpawnManager*>& OutManagers);
	void AssignPlayerToSlot(class ACSPlayerState* PlayerState, ESpawnSlotType SlotType, const TArray<class ACSSpawnManager*>& Managers);
	void SpawnOrUpdateLobbyCharacter(APlayerController* PlayerController, const FVector& Location, const FRotator& Rotation);
};

