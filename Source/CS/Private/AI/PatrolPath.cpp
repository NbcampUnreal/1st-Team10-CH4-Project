#include "AI/PatrolPath.h"

APatrolPath::APatrolPath()
{
	
	PrimaryActorTick.bCanEverTick = false;
}

FVector APatrolPath::GetPatrolPoint(int const index) const
{
	return PatrolPoints[index];
}

int APatrolPath::Num() const
{
	return PatrolPoints.Num();
}
