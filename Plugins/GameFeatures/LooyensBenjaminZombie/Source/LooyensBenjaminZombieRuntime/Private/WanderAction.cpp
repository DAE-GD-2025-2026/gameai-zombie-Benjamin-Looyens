#include "WanderAction.h"
#include "SurvivorDecisionMaker.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>

WanderAction::WanderAction()
{
	m_pWander = MakeUnique<Wander>();
	m_pPathFollow = MakeUnique<PathFollow>();

	TArray<BlendedSteering::WeightedBehavior> behaviors{
		BlendedSteering::WeightedBehavior{ m_pWander.Get(), 0.4f },
		BlendedSteering::WeightedBehavior{ m_pPathFollow.Get(), 0.6f }
	};
	m_pBehavior = MakeUnique<BlendedSteering>(behaviors);

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

	if (m_pPathFollow->HasFinishedPath(*(pSurvivor))) {
		const FVector& survivorPos = pSurvivor->GetActorLocation();
		
		FVector influencedPos = GeneratePos();

		if (!memory.exploredLocations.IsEmpty()) {
			constexpr int NUM_IMPROVED_ATTEMPTS = 15;

			TArray<FVector> possibleLocations{ influencedPos };
			TArray<float> locationWeights{ CalculateWeight(influencedPos, memory) };
			possibleLocations.Reserve(NUM_IMPROVED_ATTEMPTS);
			locationWeights.Reserve(NUM_IMPROVED_ATTEMPTS);

			float totalWeight{ locationWeights[0] };

			for (int index{}; index < NUM_IMPROVED_ATTEMPTS - 1; index++) {
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

		influencedPos.Z = survivorPos.Z;
		memory.exploredLocations.Add(influencedPos);
		TArray<FVector> path = pSurvivor->CalculatePath(influencedPos);
		m_pPathFollow->SetPath(path);
		memory.explorePathDirty = false;

		UE_LOG(LogTemp, Log, TEXT("Calculated New Wander Point: [%f, %f, %f]"), influencedPos.X, influencedPos.Y, influencedPos.Z);
		UE_LOG(LogTemp, Log, TEXT("Path to point has [%i] points in path"), path.Num());
	}
	else if (memory.explorePathDirty) {
		TArray<FVector> path = pSurvivor->CalculatePath(memory.exploredLocations.Last());
		m_pPathFollow->SetPath(path);
		memory.explorePathDirty = false;

		UE_LOG(LogTemp, Log, TEXT("Recalculated Wander Path"));
	}

	const auto steering = m_pBehavior->CalculateSteering(pWorld->GetDeltaSeconds(), *(pSurvivor));
	ISteeringBehavior::ApplySteering(pSurvivor, steering);
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
