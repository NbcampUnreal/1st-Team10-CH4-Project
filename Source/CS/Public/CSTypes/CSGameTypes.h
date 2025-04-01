#pragma once

#include "CoreMinimal.h"
#include "CSGameTypes.generated.h"

// 예시
UENUM(BlueprintType)
enum class EMatchType : uint8
{
	Single,
	Versus,
	Coop
};

//USTRUCT(BlueprintType)
//struct FSinglePlaySettings
//{
//    GENERATED_BODY()
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    int32 DifficultyLevel;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    FName StartMapName;
//};