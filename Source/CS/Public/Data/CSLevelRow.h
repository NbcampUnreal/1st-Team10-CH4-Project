#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CSLevelRow.generated.h"


USTRUCT(BlueprintType)
struct FLevelRow : public FTableRowBase
{
	GENERATED_BODY()

	// 실제 에디터에서 만든 레벨 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName LevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> MapAsset;
	// 게임 내 UI나 설명에 사용할 표시 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
};
