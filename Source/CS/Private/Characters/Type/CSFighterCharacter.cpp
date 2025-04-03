// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Type/CSFighterCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	}
}

/* Combo 1 */
void ACSFighterCharacter::PlayPCombo1Montage()
{
	iCombo_2_Cnt++;
	PlayPlayerMontage(P_AttackMontage, "Punch1");
}

void ACSFighterCharacter::PlayPCombo2Montage()
{
	iCombo_2_Cnt++;
	PlayPlayerMontage(P_AttackMontage, "Punch2");
}

void ACSFighterCharacter::PlayPCombo3Montage()
{
	PlayPlayerMontage(P_AttackMontage, "Punch3");
}

void ACSFighterCharacter::PlayPComboAnim()
{
	if (!bCanCombo) return;

	if (iCombo_2_Cnt == 0) PlayPCombo1Montage();
	else if (iCombo_2_Cnt == 1) PlayPCombo2Montage();
	else if (iCombo_2_Cnt == 2) PlayPCombo3Montage();
}

/* Combo 2 */
void ACSFighterCharacter::PlayKCombo1Montage()
{
	iCombo_1_Cnt++;
	PlayPlayerMontage(K_AttackMontage, "Kick1");
}

void ACSFighterCharacter::PlayKCombo2Montage()
{
	iCombo_1_Cnt++;
	PlayPlayerMontage(K_AttackMontage, "Kick2");
}

void ACSFighterCharacter::PlayKCombo3Montage()
{
	PlayPlayerMontage(K_AttackMontage, "Kick3");
}

void ACSFighterCharacter::PlayKComboAnim()
{
	if (!bCanCombo) return;

	if (iCombo_1_Cnt == 0) PlayKCombo1Montage();
	else if (iCombo_1_Cnt == 1) PlayKCombo2Montage();
	else if (iCombo_1_Cnt == 2) PlayKCombo3Montage();
}

