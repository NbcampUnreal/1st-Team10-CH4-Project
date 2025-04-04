// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Type/CSFighterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CSCombatComponent.h"

ACSFighterCharacter::ACSFighterCharacter()
{
}

void ACSFighterCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FighterMappingContext, 1);
		}
	}
}

void ACSFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACSFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(P_ComboAction, ETriggerEvent::Started, this, &ACSFighterCharacter::PlayPComboAnim);
		EnhancedInputComponent->BindAction(K_ComboAction, ETriggerEvent::Started, this, &ACSFighterCharacter::PlayKComboAnim);
		EnhancedInputComponent->BindAction(P_Casting, ETriggerEvent::Started, this, &ACSFighterCharacter::PlayPCastingAnim);
		EnhancedInputComponent->BindAction(K_Casting, ETriggerEvent::Started, this, &ACSFighterCharacter::PlayKCastingAnim);
	}
}

/* Combo 1 */
void ACSFighterCharacter::PlayPCombo1Montage()
{
	CombatComponent->Combo1CntIncrease();
	PlayPlayerMontage(P_AttackMontage, "Punch1");
}

void ACSFighterCharacter::PlayPCombo2Montage()
{
	CombatComponent->Combo1CntIncrease();
	PlayPlayerMontage(P_AttackMontage, "Punch2");
}

void ACSFighterCharacter::PlayPCombo3Montage()
{
	PlayPlayerMontage(P_AttackMontage, "Punch3");
}

void ACSFighterCharacter::PlayPComboAnim()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%d"), CombatComponent->GetCanCombo()));

	if (!CombatComponent->GetCanCombo()) return;

	if (CombatComponent->GetCombo1Cnt() == 0) PlayPCombo1Montage();
	else if (CombatComponent->GetCombo1Cnt() == 1) PlayPCombo2Montage();
	else if (CombatComponent->GetCombo1Cnt() == 2) PlayPCombo3Montage();
}

/* Combo 2 */
void ACSFighterCharacter::PlayKCombo1Montage()
{
	CombatComponent->Combo2CntIncrease();
	PlayPlayerMontage(K_AttackMontage, "Kick1");
}

void ACSFighterCharacter::PlayKCombo2Montage()
{
	CombatComponent->Combo2CntIncrease();
	PlayPlayerMontage(K_AttackMontage, "Kick2");
}

void ACSFighterCharacter::PlayKCombo3Montage()
{
	PlayPlayerMontage(K_AttackMontage, "Kick3");
}

void ACSFighterCharacter::PlayKComboAnim()
{
	if (!CombatComponent->GetCanCombo()) return;

	if (CombatComponent->GetCombo2Cnt() == 0) PlayKCombo1Montage();
	else if (CombatComponent->GetCombo2Cnt() == 1) PlayKCombo2Montage();
	else if (CombatComponent->GetCombo2Cnt() == 2) PlayKCombo3Montage();
}

/* Casting */
void ACSFighterCharacter::PlayPCastingAnim()
{
	if (!CombatComponent->GetCanCombo()) return;

	PlayPlayerMontage(P_CastingMontage, "Default");
}

void ACSFighterCharacter::PlayKCastingAnim()
{
	if (!CombatComponent->GetCanCombo()) return;

	PlayPlayerMontage(K_CastingMontage, "Default");
}

