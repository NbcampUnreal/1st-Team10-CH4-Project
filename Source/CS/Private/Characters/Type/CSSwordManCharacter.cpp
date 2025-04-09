// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Type/CSSwordManCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "Components/CSCombatComponent.h"

ACSSwordManCharacter::ACSSwordManCharacter()
{

}

void ACSSwordManCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SwordManMappingContext, 1);
		}
	}
}

void ACSSwordManCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSSwordManCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(Lignt_ComboAction, ETriggerEvent::Started, this, &ACSSwordManCharacter::PlayLightComboAnim);
		//EnhancedInputComponent->BindAction(K_ComboAction, ETriggerEvent::Started, this, &ACSFighterCharacter::PlayKComboAnim);
		EnhancedInputComponent->BindAction(Lignt_Casting, ETriggerEvent::Started, this, &ACSSwordManCharacter::PlayLightCastAnim);
		//EnhancedInputComponent->BindAction(K_Casting, ETriggerEvent::Started, this, &ACSFighterCharacter::PlayKCastingAnim);
	}
}

/* Combo 1 */
void ACSSwordManCharacter::PlayLightComboMontage()
{
	int32 iCnt = CombatComponent->GetCombo1Cnt();
	PlayPlayerMontage(LightMontage[iCnt].AttackMontage, LightMontage[iCnt].Section);
	CombatComponent->Combo1CntIncrease();
}

void ACSSwordManCharacter::PlayLightComboAnim()
{
	if (!CombatComponent->GetCanCombo()) return;

	PlayLightComboMontage();
}

/* Casting */
void ACSSwordManCharacter::PlayLightCastAnim()
{
	if (!CombatComponent->GetCanCombo()) return;

	PlayPlayerMontage(LightCastMontage[0].AttackMontage, LightCastMontage[0].Section);
}
