#include "SteeringHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"

FVector2D SteeringPawnAccess::GetPawnPosition(const APawn& pPawn)
{
    return FVector2D{ pPawn.GetActorLocation().X, pPawn.GetActorLocation().Y };
}

float SteeringPawnAccess::GetPawnRotation(const APawn& pPawn)
{
    return pPawn.GetActorRotation().Yaw;
}

float SteeringPawnAccess::GetPawnMaxLinearSpeed(const APawn& pPawn)
{
    return pPawn.GetMovementComponent()->GetMaxSpeed();
}

FVector2D SteeringPawnAccess::GetPawnLinearVelocity(const APawn& pPawn)
{
    return FVector2D{ pPawn.GetMovementComponent()->Velocity };
}