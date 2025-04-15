#include "GameInstance/CSAdvancedGameInstance.h"
#include "Controller/CSPlayerController.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSubsystemUtils.h"
#include "Data/CSLevelRow.h"
#include "Data/CSCharacterRow.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/NetDriver.h" 

void UCSAdvancedGameInstance::Init()
{
	Super::Init();

	MatchType = EMatchType::EMT_MainMenu;
	ExpectedPlayerCount = 0;
	bAutoHostIfNoSession = false;
}

void UCSAdvancedGameInstance::SetMatchType(EMatchType NewType)
{
	MatchType = NewType;
}

void UCSAdvancedGameInstance::ResetLobbySettings()
{
	ExpectedPlayerCount = 0;
	PlayerLobbyDataMap.Empty();
}

void UCSAdvancedGameInstance::HostServerTravel()
{
	if (UWorld* World = GetWorld())
	{
		World->ServerTravel(TEXT("/Game/Blueprints/Hud/Maps/LobbyLevel?listen"));
	}
}

void UCSAdvancedGameInstance::StartArcadeMode()
{
	SetMatchType(EMatchType::EMT_Single);
	UGameplayStatics::OpenLevel(this, FName("/Game/Blueprints/Hud/Maps/SingleModeLevel"));
}

void UCSAdvancedGameInstance::ServerTravelToLevel(FName LevelID)
{
	if (!LevelData) return;
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, TEXT("")))
	{
		GetWorld()->ServerTravel(Row->MapPath + TEXT("?listen"));
	}
}

void UCSAdvancedGameInstance::OpenLevelByID(FName LevelID)
{
	if (!LevelData) return;
	if (const FLevelRow* Row = LevelData->FindRow<FLevelRow>(LevelID, TEXT("")))
	{
		UGameplayStatics::OpenLevel(this, FName(*Row->MapPath));
	}
}

void UCSAdvancedGameInstance::SetPlayerLobbyData(const FString& PlayerName, const FPlayerLobbyData& Data)
{
	PlayerLobbyDataMap.Add(PlayerName, Data);
}

FPlayerLobbyData UCSAdvancedGameInstance::GetPlayerLobbyData(const FString& PlayerName) const
{
	if (const FPlayerLobbyData* Found = PlayerLobbyDataMap.Find(PlayerName))
	{
		return *Found;
	}
	return FPlayerLobbyData();
}

const FCharacterRow* UCSAdvancedGameInstance::FindCharacterRowByJob(EJobTypes Job) const
{
	if (!CharacterData) return nullptr;
	FString EnumString = StaticEnum<EJobTypes>()->GetNameStringByValue((int64)Job);
	EnumString.RemoveFromStart("EJT_");
	return CharacterData->FindRow<FCharacterRow>(FName(*EnumString), TEXT(""));
}

const FLevelRow* UCSAdvancedGameInstance::FindLevelRow(FName RowName) const
{
	if (!LevelData) return nullptr;
	return LevelData->FindRow<FLevelRow>(RowName, TEXT("FindLevelRow"));
}
