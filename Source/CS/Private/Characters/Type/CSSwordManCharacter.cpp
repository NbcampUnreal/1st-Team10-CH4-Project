// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Type/CSSwordManCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "Components/CSCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ACSSwordManCharacter::ACSSwordManCharacter()
{

}

void ACSSwordManCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the CombatComponent if not already set
	
}

void ACSSwordManCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSSwordManCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!CombatComponent)
	{
		CombatComponent = FindComponentByClass<UCSCombatComponent>();
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SwordManMappingContext, 1);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(Light_ComboAction, ETriggerEvent::Started, this, &ACSSwordManCharacter::HandleLightAttackPress);
		EnhancedInputComponent->BindAction(Light_ComboAction, ETriggerEvent::Completed, this, &ACSSwordManCharacter::HandleLightAttackRelease);
		EnhancedInputComponent->BindAction(Heavy_ComboAction, ETriggerEvent::Started, this, &ACSSwordManCharacter::PlayHeavyAttackAnim);
		EnhancedInputComponent->BindAction(Combination_Action, ETriggerEvent::Started, this, &ACSSwordManCharacter::PlayCombinationAnim);
	}
}

void ACSSwordManCharacter::HandleLightAttackPress()
{
	if (CombatComponent && CombatComponent->GetCanCombo())
	{
		bIsLightAttackPressed = true;
		bPerformedCounter = false;

		GetWorldTimerManager().SetTimer(LightAttackHoldTimer, this, &ACSSwordManCharacter::CheckForCounterAttack, CounterHoldThreshold, false);
	}
}

void ACSSwordManCharacter::HandleLightAttackRelease()
{
	if (!bIsLightAttackPressed) return;

	bIsLightAttackPressed = false;

	if (GetWorldTimerManager().IsTimerActive(LightAttackHoldTimer))
	{
		GetWorldTimerManager().ClearTimer(LightAttackHoldTimer);
	}

	if (!bPerformedCounter)
	{
		PlayLightComboMontage();
	}
}

void ACSSwordManCharacter::CheckForCounterAttack()
{
	if (bIsLightAttackPressed && CombatComponent && CombatComponent->GetCanCombo())
	{
		if (CounterMontage)
		{
			bPerformedCounter = true;
			PlayPlayerMontage(CounterMontage, FName("Counter"));
			CombatComponent->SetCurrentAttackDamage(CounterAttackDamage);
			CombatComponent->ResetComboData();
		}
	}
}

void ACSSwordManCharacter::PlayLightComboMontage()
{
	if (!CombatComponent || !CombatComponent->GetCanCombo() || LightMontage.Num() <= 0) return;

	int32 iCnt = CombatComponent->GetCombo1Cnt();

	if (LightMontage.IsValidIndex(iCnt))
	{
		const FAttackMontageStruct& CurrentAttackData = LightMontage[iCnt];

		if (CurrentAttackData.AttackMontage)
		{
			PlayPlayerMontage(CurrentAttackData.AttackMontage, CurrentAttackData.Section);
			CombatComponent->SetCurrentAttackDamage(CurrentAttackData.Damage);
			CombatComponent->Combo1CntIncrease();
		}
		else
		{
			CombatComponent->ResetComboData();
		}
	}
	else
	{
		CombatComponent->ResetComboData();
	}
}

void ACSSwordManCharacter::PlayHeavyAttackAnim()
{
	if (!CombatComponent || !CombatComponent->GetCanCombo() || HeavyMontage.Num() <= 0) return;

	int32 iCnt = CombatComponent->GetCombo1Cnt();

	if (HeavyMontage.IsValidIndex(iCnt))
	{
		const FAttackMontageStruct& CurrentAttackData = HeavyMontage[iCnt];

		if (CurrentAttackData.AttackMontage)
		{
			PlayPlayerMontage(CurrentAttackData.AttackMontage, CurrentAttackData.Section);
			CombatComponent->SetCurrentAttackDamage(CurrentAttackData.Damage);
			CombatComponent->Combo1CntIncrease();
		}
		else
		{
			CombatComponent->ResetComboData();
		}
	}
	else
	{
		CombatComponent->ResetComboData();
	}
}

void ACSSwordManCharacter::PlayCombinationAnim()
{
	if (!CombatComponent || !CombatComponent->GetCanCombo() || CombinationMontage.Num() <= 0) return;

	int32 iCnt = CombatComponent->GetCombo1Cnt();

	if (CombinationMontage.IsValidIndex(iCnt))
	{
		const FAttackMontageStruct& CurrentAttackData = CombinationMontage[iCnt];

		if (CurrentAttackData.AttackMontage)
		{
			PlayPlayerMontage(CurrentAttackData.AttackMontage, CurrentAttackData.Section);
			CombatComponent->SetCurrentAttackDamage(CurrentAttackData.Damage);
			CombatComponent->Combo1CntIncrease();
		}
		else
		{
			CombatComponent->ResetComboData();
		}
	}
	else
	{
		CombatComponent->ResetComboData();
	}
}
