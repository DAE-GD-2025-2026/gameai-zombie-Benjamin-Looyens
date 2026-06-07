#include "WanderAction.h"
#include "SurvivorDecisionMaker.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>

WanderAction::WanderAction()
{
	m_pBehavior = MakeUnique<Wander>();
	m_pBehaviorPath = MakeUnique<PathFollow>();

	UE_LOG(LogTemp, Log, TEXT("Wander Action Created"));
}

float WanderAction::Evaluate(const SurvivorMemory& memory)
{
	// 1.0f baseline
	// wander when nothing works
	return 1.0f; 
}

void WanderAction::Execute(SurvivorMemory& memory)
{
	auto& pSurvivor = memory.pSurvivor;
	const auto& pWorld = pSurvivor->GetWorld();

	if (m_pBehaviorPath->HasFinishedPath(*(pSurvivor))) {
		const FVector& survivorPos = pSurvivor->GetActorLocation();
		
		FVector influencedPos = survivorPos + FMath::VRand() * FMath::RandRange(s_WANDER_TARGET_MIN, s_WANDER_TARGET_MAX); // TODO : Maybe center this on Origin of the map rather than the survivor? (and thus make the ranges larger)

		if (!memory.exploredLocations.IsEmpty()) {
			// TODO : Influence the position to not be in the same place

		}

		memory.exploredLocations.Add(influencedPos);
		TArray<FVector> path = pSurvivor->CalculatePath(influencedPos);
		m_pBehaviorPath->SetPath(path);
		memory.explorePathDirty = false;
	} 
	else if (memory.explorePathDirty) {
		TArray<FVector> path = pSurvivor->CalculatePath(memory.exploredLocations.Last());
		m_pBehaviorPath->SetPath(path);
		memory.explorePathDirty = false;
	}

	//const auto steering = m_pBehavior->CalculateSteering(pWorld->GetDeltaSeconds(), *(pSurvivor));
	const auto steering = m_pBehaviorPath->CalculateSteering(pWorld->GetDeltaSeconds(), *(pSurvivor));
	ISteeringBehavior::ApplySteering(pSurvivor, steering);
}

void WanderAction::LateExecute(SurvivorMemory& memory)
{
}
