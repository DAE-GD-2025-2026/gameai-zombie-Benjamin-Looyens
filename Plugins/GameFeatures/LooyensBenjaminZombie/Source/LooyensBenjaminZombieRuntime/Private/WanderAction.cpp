#include "WanderAction.h"
#include "SurvivorDecisionMaker.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>

WanderAction::WanderAction()
{
	m_pBehavior = MakeUnique<Wander>();

	UE_LOG(LogTemp, Log, TEXT("Wander Action Created"));
}

float WanderAction::Evaluate(const SurvivorMemory& memory)
{
	// 1.0f baseline
	// wander when nothing works
	return 1.0f; 
}

void WanderAction::Execute(const SurvivorMemory& memory)
{
	const auto& pWorld = memory.pSurvivor->GetWorld();

	const auto steering = m_pBehavior->CalculateSteering(pWorld->GetDeltaSeconds(), *(memory.pSurvivor));
	ISteeringBehavior::ApplySteering(memory.pSurvivor, steering);
}
