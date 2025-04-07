#pragma once

#include "CoreMinimal.h"
#include "CSGameTypes.generated.h"

UENUM(BlueprintType)
enum class EMatchType : uint8
{
	EMT_None        UMETA(DisplayName = "None"),
    EMT_MainMenu    UMETA(DisplayName = "MainMenu"),
	EMT_Single      UMETA(DisplayName = "SinglePlay"),
	EMT_Versus      UMETA(DisplayName = "Versus"),
	EMT_Coop        UMETA(DisplayName = "Coop")
};

UENUM(BlueprintType)
enum class EMatchPhase : uint8
{
    EMP_None       UMETA(DisplayName = "None"),
	EMP_Waiting    UMETA(DisplayName = "Waiting"),
	EMP_Playing    UMETA(DisplayName = "Playing"),
	EMP_Finished   UMETA(DisplayName = "Finished")
};

UENUM(BlueprintType)
enum class EMatchResult : uint8
{
    EMR_None   UMETA(DisplayName = "None"),
    EMR_Win    UMETA(DisplayName = "Win"),
    EMR_Lose   UMETA(DisplayName = "Lose")
};

UENUM(BlueprintType)
enum class ESpawnSlotType : uint8
{
    None                    UMETA(DisplayName = "None"),
    // 대전모드 팀 0 (왼쪽)
    Versus_Team0_Slot0      UMETA(DisplayName = "Versus - Team0 - Slot0"),
    Versus_Team0_Slot1      UMETA(DisplayName = "Versus - Team0 - Slot1"),
    Versus_Team0_Slot2      UMETA(DisplayName = "Versus - Team0 - Slot2"),

    // 대전모드 팀 1 (오른쪽)
    Versus_Team1_Slot0      UMETA(DisplayName = "Versus - Team1 - Slot0"),
    Versus_Team1_Slot1      UMETA(DisplayName = "Versus - Team1 - Slot1"),
    Versus_Team1_Slot2      UMETA(DisplayName = "Versus - Team1 - Slot2"),

    // 협동모드 (Player)
    Coop_Player_Slot0       UMETA(DisplayName = "Coop - Player - Slot0"),
    Coop_Player_Slot1       UMETA(DisplayName = "Coop - Player - Slot1"),
    Coop_Player_Slot2       UMETA(DisplayName = "Coop - Player - Slot2"),
    Coop_Player_Slot3       UMETA(DisplayName = "Coop - Player - Slot3"),

    // 협동모드 (AI)
    Coop_AI_Slot0           UMETA(DisplayName = "Coop - AI - Slot0"),
    Coop_AI_Slot1           UMETA(DisplayName = "Coop - AI - Slot1"),
    Coop_AI_Slot2           UMETA(DisplayName = "Coop - AI - Slot2"),
    Coop_AI_Slot3           UMETA(DisplayName = "Coop - AI - Slot3"),
};