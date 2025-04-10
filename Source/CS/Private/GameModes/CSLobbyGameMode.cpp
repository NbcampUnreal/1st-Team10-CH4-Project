#include "GameModes/CSLobbyGameMode.h"
#include "GameInstance/CSGameInstance.h"
#include "GameStates/CSLobbyGameState.h"
#include "PlayerStates/CSPlayerState.h"
#include "Controller/CSPlayerController.h"
#include "Characters/Type/CSLobbyCharacter.h"
#include "Managers/CSSpawnManager.h"
#include "Camera/CameraActor.h"
#include "Data/CSLevelRow.h"
#include "Kismet/GameplayStatics.h"

ACSLobbyGameMode::ACSLobbyGameMode()
{
	GameStateClass = ACSLobbyGameState::StaticClass();
	PlayerStateClass = ACSPlayerState::StaticClass();
	DefaultPawnClass = nullptr;

	MatchType = EMatchType::EMT_None;
}

void ACSLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (const UCSGameInstance* CSGameInstance = GetGameInstance<UCSGameInstance>())
	{
		MatchType = CSGameInstance->GetMatchType();
	}
}

void ACSLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(NewPlayer))
	{
		CSPlayerController->Client_ShowLobbyUI();
	}

	if (MatchType == EMatchType::EMT_Versus)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(NewPlayer->PlayerState))
		{
			int32 Num = GameState.Get()->PlayerArray.Num();

			CSPlayerState->PlayerIndex = Num;
			CSPlayerState->TeamID = (Num % 2 == 0) ? 1 : 0;
		}
	}

	FTimerHandle DelayHandle;
	GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this, NewPlayer]()
		{
			UE_LOG(LogTemp, Warning, TEXT("=== Delayed Setup Executing for %s ==="), *NewPlayer->GetName());

			SetViewLobbyCam(NewPlayer);
			SetSelectedPlayerJob(NewPlayer, EJobTypes::EJT_Fighter);
			PositionLobbyCharacters();

		}, 1.0f, false);
}

void ACSLobbyGameMode::StartMatchIfReady()
{
	const ACSLobbyGameState* LobbyGameState = GetGameState<ACSLobbyGameState>();
	if (!LobbyGameState) return;

	if (!LobbyGameState->AreAllPlayerReady()) return;

	switch (MatchType)
	{
	case EMatchType::EMT_Versus:
		if (NumPlayers >= 2 && NumPlayers % 2 == 0 && IsTeamBalanced())
		{
			TryStartMatch();
		}
		break;

	case EMatchType::EMT_Coop:
		if (NumPlayers >= 2 && NumPlayers <= 4)
		{
			TryStartMatch();
		}
		break;

	default:
		break;
	}
}

void ACSLobbyGameMode::TryStartMatch()
{
	const ACSLobbyGameState* LobbyGameState = GetGameState<ACSLobbyGameState>();
	if (!LobbyGameState || LobbyGameState->SelectedMap == NAME_None) return;

	if (UCSGameInstance* CSGameInstance = GetGameInstance<UCSGameInstance>())
	{
		const int32 PlayerCount = GameState->PlayerArray.Num();
		CSGameInstance->ExpectedPlayerCount = PlayerCount;

		if (CSGameInstance->LevelData)
		{
			const FString ContextStr = TEXT("TryStartMatch");
			const FLevelRow* LevelRow = CSGameInstance->LevelData->FindRow<FLevelRow>(LobbyGameState->SelectedMap, ContextStr);
			if (LevelRow && !LevelRow->MapPath.IsEmpty())
			{
				const FString TravelURL = LevelRow->MapPath + TEXT("?listen");

				bUseSeamlessTravel = true;
				GetWorld()->ServerTravel(TravelURL);
			}
		}
	}
}

void ACSLobbyGameMode::ChangeTeam(APlayerController* Player)
{
	if (ACSPlayerState* CSPlayerState = Player->GetPlayerState<ACSPlayerState>())
	{
		int32 CurrentTeam = CSPlayerState->TeamID;
		int32 NewTeam = (CurrentTeam == 0) ? 1 : 0;

		CSPlayerState->TeamID = NewTeam;

		PositionLobbyCharacters();
	}
}

void ACSLobbyGameMode::PositionLobbyCharacters()
{
	switch (MatchType)
	{
	case EMatchType::EMT_Versus:
		PositionVersusCharacters();
		break;

	case EMatchType::EMT_Coop:
		PositionCoopCharacters();
		break;

	default:
		break;
	}
}

void ACSLobbyGameMode::PositionVersusCharacters()
{
	const auto TeamSlotPriority = GetSlotPriorityForMatchType();
	UE_LOG(LogTemp, Warning, TEXT("포지션 벌서스 캐릭터 함수 호출됨"));
	TMap<int32, TArray<ACSPlayerState*>> TeamMap;
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			TeamMap.FindOrAdd(CSPlayerState->TeamID).Add(CSPlayerState);
		}
	}

	TArray<ACSSpawnManager*> SpawnManagers;
	GetAllSpawnManagers(SpawnManagers);

	if (SpawnManagers.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("스폰매니저 못찾음"));
	}

	for (auto& Entry : TeamMap)
	{
		int32 TeamID = Entry.Key;
		TArray<ACSPlayerState*>& Players = Entry.Value;

		for (int32 i = 0; i < Players.Num(); ++i)
		{
			if (TeamSlotPriority.Contains(TeamID) && i < TeamSlotPriority[TeamID].Num())
			{
				AssignPlayerToSlot(Players[i], TeamSlotPriority[TeamID][i], SpawnManagers);
			}
		}
	}
}

void ACSLobbyGameMode::PositionCoopCharacters()
{
	TMap<int32, TArray<ESpawnSlotType>> SlotMap = GetSlotPriorityForMatchType();
	if (!SlotMap.Contains(0)) return;
	const TArray<ESpawnSlotType>& SlotList = SlotMap[0];

	TArray<ACSSpawnManager*> SpawnManagers;
	GetAllSpawnManagers(SpawnManagers);

	int32 Index = 0;
	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			if (Index >= SlotList.Num()) break;
			AssignPlayerToSlot(CSPlayerState, SlotList[Index], SpawnManagers);
			++Index;
		}
	}
}

TMap<int32, TArray<ESpawnSlotType>> ACSLobbyGameMode::GetSlotPriorityForMatchType() const
{
	TMap<int32, TArray<ESpawnSlotType>> Result;

	switch (MatchType)
	{
	case EMatchType::EMT_Versus:
		Result.Add(0, { ESpawnSlotType::Versus_Team0_Slot0, ESpawnSlotType::Versus_Team0_Slot1 });
		Result.Add(1, { ESpawnSlotType::Versus_Team1_Slot0, ESpawnSlotType::Versus_Team1_Slot1 });
		break;

	case EMatchType::EMT_Coop:
		Result.Add(0, {
			ESpawnSlotType::Coop_Player_Slot0,
			ESpawnSlotType::Coop_Player_Slot1,
			ESpawnSlotType::Coop_Player_Slot2,
			ESpawnSlotType::Coop_Player_Slot3 });
		break;

	default:
		break;
	}

	return Result;
}

void ACSLobbyGameMode::GetAllSpawnManagers(TArray<ACSSpawnManager*>& OutManagers)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSSpawnManager::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (ACSSpawnManager* SpawnManager = Cast<ACSSpawnManager>(Actor))
		{
			OutManagers.Add(SpawnManager);
		}
	}
}

void ACSLobbyGameMode::AssignPlayerToSlot(ACSPlayerState* PlayerState, ESpawnSlotType SlotType, const TArray<ACSSpawnManager*>& Managers)
{
	for (ACSSpawnManager* Manager : Managers)
	{
		if (Manager->SlotType == SlotType)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(PlayerState->GetOwner()))
			{
				SpawnOrUpdateLobbyCharacter(PlayerController, Manager->GetActorLocation(), Manager->GetActorRotation());
			}
			break;
		}
	}
}

void ACSLobbyGameMode::SpawnOrUpdateLobbyCharacter(APlayerController* PlayerController, const FVector& Location, const FRotator& Rotation)
{
	if (!PlayerController) return;

	ACSLobbyCharacter* LobbyCharacter = nullptr;

	if (LobbyCharacterMap.Contains(PlayerController))
	{
		LobbyCharacter = LobbyCharacterMap[PlayerController];
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = PlayerController;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		LobbyCharacter = GetWorld()->SpawnActor<ACSLobbyCharacter>(ACSLobbyCharacter::StaticClass(), Location, Rotation, SpawnParams);
		if (LobbyCharacter)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawned LobbyCharacter for %s"), *PlayerController->GetName());
			LobbyCharacterMap.Add(PlayerController, LobbyCharacter);
		}
	}

	if (LobbyCharacter)
	{
		LobbyCharacter->SetActorLocation(Location);
		LobbyCharacter->SetActorRotation(Rotation);
	}
}

void ACSLobbyGameMode::SetSelectedPlayerJob(APlayerController* Player, EJobTypes NewJob)
{
	if (ACSPlayerState* CSPlayerState = Player->GetPlayerState<ACSPlayerState>())
	{
		CSPlayerState->SelectedJob = NewJob;

		if (ACSLobbyCharacter* LobbyCharacter = LobbyCharacterMap.FindRef(Player))
		{
			LobbyCharacter->UpdateMeshFromJobType(NewJob);
		}
	}
}

bool ACSLobbyGameMode::IsTeamBalanced()
{
	int32 Team_0 = 0;
	int32 Team_1 = 0;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (ACSPlayerState* CSPlayerState = Cast<ACSPlayerState>(PlayerState))
		{
			(CSPlayerState->TeamID == 0) ? Team_0++ : Team_1++;
		}
	}

	return Team_0 == Team_1;
}

void ACSLobbyGameMode::SetViewLobbyCam(APlayerController* NewPlayer)
{
	FName CameraTag;
	UE_LOG(LogTemp,Warning, TEXT("LobbyGameMode: SetViewLobbyCam Called"))
	switch (MatchType)
	{
	case EMatchType::EMT_Versus:
		CameraTag = FName("LobbyCamera_Versus");
		break;

	case EMatchType::EMT_Coop:
		CameraTag = FName("LobbyCamera_Coop");
		break;
	}

	TArray<AActor*> FoundCameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CameraTag, FoundCameras);

	if (FoundCameras.Num() > 0)
	{
		if (ACameraActor* LobbyCam = Cast<ACameraActor>(FoundCameras[0]))
		{
			NewPlayer->SetViewTarget(LobbyCam);
		}
	}
}