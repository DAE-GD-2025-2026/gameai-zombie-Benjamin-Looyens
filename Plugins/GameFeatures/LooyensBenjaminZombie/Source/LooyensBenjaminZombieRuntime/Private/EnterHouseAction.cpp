#include "EnterHouseAction.h"
#include "SurvivorDecisionMaker.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Village/House/House.h>

EnterHouseAction::EnterHouseAction()
{
	m_pBehavior = MakeUnique<PathFollow>();

	UE_LOG(LogTemp, Log, TEXT("Enter House Action Created"));
}

float EnterHouseAction::Evaluate(const SurvivorMemory& memory)
{
	float value = memory.houses.Num() * 10.0f; // The more houses seen, the higher the value

	// Stuff to modify value:
	// If there are nearby houses
	// How full inventory is
	// HP / Stamina state
	// If being chased?

	return value;
}

void EnterHouseAction::Execute(const SurvivorMemory& memory)
{
	const auto& pWorld = memory.pSurvivor->GetWorld();

	if (!m_PathIsUpToDate) {
		const auto path = memory.pSurvivor->CalculatePath(memory.houses[0].ptr->GetActorLocation());

		m_pBehavior->SetPath(path);

		m_PathIsUpToDate = true;
	}

	const auto steering = m_pBehavior->CalculateSteering(pWorld->GetDeltaSeconds(), *(memory.pSurvivor));
	ISteeringBehavior::ApplySteering(memory.pSurvivor, steering);
}
