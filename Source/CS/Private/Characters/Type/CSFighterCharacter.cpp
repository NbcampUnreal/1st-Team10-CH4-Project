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

	if (!CombatComponent)
	{
		CombatComponent = FindComponentByClass<UCSCombatComponent>();
	}

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
void ACSFighterCharacter::PlayPComboMontage()
{
	int32 iCnt = CombatComponent->GetCombo1Cnt();
	
	PlayPlayerMontage(PunchMontage[iCnt].AttackMontage, PunchMontage[iCnt].Section);
	CombatComponent->SetCurrentAttackDamage(PunchMontage[iCnt].Damage);
	CombatComponent->Combo1CntIncrease();
}

void ACSFighterCharacter::PlayPComboAnim()
{
	if (!CombatComponent->GetCanCombo()) return;

	PlayPComboMontage();
}

/* Combo 2 */
void ACSFighterCharacter::PlayKComboMontage()
{
	int32 iCnt = CombatComponent->GetCombo2Cnt();
	PlayPlayerMontage(KickMontage[iCnt].AttackMontage, KickMontage[iCnt].Section);
	CombatComponent->SetCurrentAttackDamage(KickMontage[iCnt].Damage);
	CombatComponent->Combo2CntIncrease();
}

void ACSFighterCharacter::PlayKComboAnim()
{
	if (!CombatComponent->GetCanCombo()) return;
	
	PlayKComboMontage();
}

/* Casting */
void ACSFighterCharacter::PlayPCastingAnim()
{
	if (!CombatComponent->GetCanCombo()) return;

	PlayPlayerMontage(PunchCastMontage[0].AttackMontage, PunchCastMontage[0].Section);
	CombatComponent->SetCurrentAttackDamage(PunchCastMontage[0].Damage);
}

void ACSFighterCharacter::PlayKCastingAnim()
{
	if (!CombatComponent->GetCanCombo()) return;

	PlayPlayerMontage(KickCastMontage[0].AttackMontage, KickCastMontage[0].Section);
	CombatComponent->SetCurrentAttackDamage(KickCastMontage[0].Damage);
}

