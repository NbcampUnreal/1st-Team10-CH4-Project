#include "GameStates/CSTutorialGameState.h"
#include "Net/UnrealNetwork.h" // 리플리케이션 사용 위해 필요
#include "Controller/CSPlayerController.h" // 헤더 포함 (캐스팅 위해)
#include "Kismet/GameplayStatics.h" // 헤더 포함 (로컬 플레이어 컨트롤러 얻기 위해)

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

	 // 주석 유지: 아래 로직은 CSPlayerController에 'Client_UpdateTutorialObjectiveUI' 같은 UI 업데이트 함수가 실제로 구현되어야 사용 가능함.
		// TODO: 여기서 로컬 플레이어의 UI 위젯을 업데이트하는 로직을 넣어야 함.
		// 예시:
		/*
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0)) // 로컬 플레이어 컨트롤러 가져오기 (싱글플레이 가정)
		{
			if (ACSPlayerController* CS_PC = Cast<ACSPlayerController>(PC))
			{
				// CS_PC->Client_UpdateTutorialObjectiveUI(CurrentObjectiveText); // PlayerController에 UI 업데이트 함수 호출
			}
		}
		*/
}