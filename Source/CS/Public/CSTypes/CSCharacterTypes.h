#pragma once

UENUM(BlueprintType)
enum class ECharacterTypes : uint8
{
	ECT_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECT_Attacking UMETA(DisplayName = "Attacking"),
	ECT_Defending UMETA(DisplayName = "Defending"),
	ECT_Moving UMETA(DisplayName = "Moving"),
	ECT_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EFacingDirection : uint8
{
	EFD_FacingRight UMETA(DisplayName = "FacingRight"),
	EFD_FacingLeft UMETA(DisplayName = "FacingLeft")
};

UENUM(BlueprintType)
enum class EJobTypes : uint8
{
	EJT_Fighter UMETA(DisplayName = "Fighter"),
	EJT_SwordMan UMETA(DisplayName = "SwordMan")
};