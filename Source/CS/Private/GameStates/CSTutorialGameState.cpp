#include "GameStates/CSTutorialGameState.h"
#include "Net/UnrealNetwork.h"
#include "Controller/CSPlayerController.h"
#include "Kismet/GameplayStatics.h"

ACSTutorialGameState::ACSTutorialGameState()
{
	CurrentObjectiveText = FText::FromString(TEXT("튜토리얼 목표가 로딩되지 않았습니다."));
	bIsTutorialComplete = false;
}

void ACSTutorialGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACSTutorialGameState, CurrentObjectiveText);
	DOREPLIFETIME(ACSTutorialGameState, bIsTutorialComplete);
}

void ACSTutorialGameState::SetCurrentObjectiveText(const FText& NewText)
{
	if (HasAuthority())
	{
		CurrentObjectiveText = NewText;
		UE_LOG(LogTemp, Log, TEXT("[TutorialGS] 서버: 현재 목표 텍스트 변경됨: %s"), *CurrentObjectiveText.ToString());
		// OnRep 자동 호출됨.
	}
}

void ACSTutorialGameState::CompleteTutorial()
{
	if (HasAuthority())
	{
		if (!bIsTutorialComplete)
		{
			bIsTutorialComplete = true;
			UE_LOG(LogTemp, Log, TEXT("[TutorialGS] 서버: 튜토리얼 완료됨!"));
		}
	}
}

void ACSTutorialGameState::OnRep_CurrentObjectiveText()
{
	UE_LOG(LogTemp, Log, TEXT("[TutorialGS] 클라이언트: 현재 목표 텍스트 업데이트됨: %s"), *CurrentObjectiveText.ToString());
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (ACSPlayerController* CS_PC = Cast<ACSPlayerController>(PC))
		{
			// CS_PC->UpdateTutorialObjectiveUI(CurrentObjectiveText);
		}
	}
}