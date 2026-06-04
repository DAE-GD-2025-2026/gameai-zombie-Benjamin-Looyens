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

//float GetPawnMaxAngularSpeed(APawn* pPawn)
//{
//    return pPawn->GetMovementComponent()->RotationRate.Yaw;
//}
//void SetPawnMaxAngularSpeed(APawn* pPawn, float maxAngularSpeed)
//{
//    pPawn->GetMovementComponent()->RotationRate.Yaw = maxAngularSpeed;
//}
//float GetPawnAngularVelocity(APawn* pPawn)
//{
//    return pPawn->GetMovementComponent()->GetLastUpdateRotation().Yaw - pPawn->GetActorRotation().Yaw;
//}
//
//bool IsPawnAutoOrienting(APawn* pPawn)
//{
//    return pPawn->GetMovementComponent()->bOrientRotationToMovement;
//}
//void SetPawnIsAutoOrienting(APawn* pPawn, bool bAutoOrient)
//{
//    pPawn->GetMovementComponent()->bOrientRotationToMovement = bAutoOrient;
//}
//
//float GetPawnMass(APawn* pPawn)
//{
//    return pPawn->GetMovementComponent()->Mass;
//}
//void SetPawnMass(APawn* pPawn, float Mass)
//{
//    pPawn->GetMovementComponent()->Mass = Mass;
//}