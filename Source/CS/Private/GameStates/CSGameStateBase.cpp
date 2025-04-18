#include "GameStates/CSGameStateBase.h"
#include "Controller/CSPlayerController.h" 
#include "UI/CSUIBaseWidget.h"         
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ACSGameStateBase::ACSGameStateBase()
{
    MatchPhase = EMatchPhase::EMP_None;
    RemainingMatchTime = 0;
}

void ACSGameStateBase::OnRep_MatchPhase()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController && PlayerController->IsLocalController())
    {
        ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(PlayerController);
        if (CSPlayerController)
        {
            UCSUIBaseWidget* CurrentUI = CSPlayerController->GetCurrentUI();
            if (CurrentUI) 
            {
                CurrentUI->HandleMatchPhaseChanged(MatchPhase);
            }
        }
    }
}

void ACSGameStateBase::OnRep_RemainingMatchTime()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController && PlayerController->IsLocalController()) 
    {
        ACSPlayerController* CSPlayerController = Cast<ACSPlayerController>(PlayerController);
        if (CSPlayerController) 
        {
            UCSUIBaseWidget* CurrentUI = CSPlayerController->GetCurrentUI();
            if (CurrentUI) 
            {
                CurrentUI->UpdateMatchTimer(RemainingMatchTime);
            }
        }
    }
}

void ACSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(ACSGameStateBase, MatchPhase, COND_None, REPNOTIFY_Always); 
    DOREPLIFETIME_CONDITION_NOTIFY(ACSGameStateBase, RemainingMatchTime, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME(ACSGameStateBase, MatchResult); 
}