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
		
		FVector influencedPos = GeneratePos();

		if (!memory.exploredLocations.IsEmpty()) {
			// TODO : Influence the position to not be in the same place
			constexpr int NUM_IMPROVED_ATTEMPTS = 15;

			TArray<FVector> possibleLocations{ influencedPos };
			TArray<float> locationWeights{ CalculateWeight(influencedPos, memory) };
			possibleLocations.Reserve(15);
			locationWeights.Reserve(15);

			float totalWeight{ locationWeights[0] };

			for (int index{}; index < possibleLocations.Num() - 1; index++) {
				const FVector altPos = GeneratePos();
				const float weight = CalculateWeight(altPos, memory);

				possibleLocations.Add(altPos);
				locationWeights.Add(weight);
				
				totalWeight += weight;
			}
			
			// Weighted Random Selection
			bool needToChoose = true;
			float chosenLocation = FMath::FRandRange(0.0f, totalWeight);
			for (int index{}; index < possibleLocations.Num(); ++index) {
				chosenLocation -= locationWeights[index];

				if (chosenLocation <= 0.0f) {
					influencedPos = possibleLocations[index];
					needToChoose = false;
					break;
				}
			}

			if (needToChoose) influencedPos = possibleLocations.Last();
		}

		memory.exploredLocations.Add(influencedPos);
		TArray<FVector> path = pSurvivor->CalculatePath(influencedPos);
		m_pBehaviorPath->SetPath(path);
		memory.explorePathDirty = false;

		UE_LOG(LogTemp, Log, TEXT("Calculated New Wander Point"));
	} 
	else if (memory.explorePathDirty) {
		TArray<FVector> path = pSurvivor->CalculatePath(memory.exploredLocations.Last());
		m_pBehaviorPath->SetPath(path);
		memory.explorePathDirty = false;

		UE_LOG(LogTemp, Log, TEXT("Recalculated Wander Path"));
	}

	//const auto steering = m_pBehavior->CalculateSteering(pWorld->GetDeltaSeconds(), *(pSurvivor));
	const auto steering = m_pBehaviorPath->CalculateSteering(pWorld->GetDeltaSeconds(), *(pSurvivor));
	ISteeringBehavior::ApplySteering(pSurvivor, steering);
}

void WanderAction::LateExecute(SurvivorMemory& memory)
{
}

FVector WanderAction::GeneratePos() const
{
	return FVector{} + FMath::VRand() * FMath::RandRange(s_WANDER_TARGET_MIN, s_WANDER_TARGET_MAX);
}

float WanderAction::CalculateWeight(const FVector& location, const SurvivorMemory& memory) const
{
	float minimumDistance = FLT_MAX;

	for (const auto& explored : memory.exploredLocations) {
		minimumDistance = FMath::Min(minimumDistance, FVector::DistSquared(location, explored));
	}

	return minimumDistance;
}
