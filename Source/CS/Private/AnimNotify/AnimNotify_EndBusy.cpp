
#include "AnimNotify/AnimNotify_EndBusy.h"
#include "AI/Character/AIBaseCharacter.h"

void UAnimNotify_EndBusy::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	if (AAIBaseCharacter* AIChar = Cast<AAIBaseCharacter>(MeshComp->GetOwner()))
	{
		AIChar->OnNotify_EndBusy();
	}
}