#include "HouseActions.h"
#include "SurvivorDecisionMaker.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Village/House/House.h>
#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>
#include "SurvivorUtils.h"

// SELECT HOUSE TO ENTER
float SelectHouseAction::Evaluate(const SurvivorMemory& memory)
{
	if (memory.pSelectedHouse) return 0.0f; // TODO : Handle this better here and in the EnterHouse

	const auto& houses = memory.houses;

	int numUnlooted = Algo::Accumulate(houses, 0,
		[](int accumulated, const HouseMemory& house) {
			return accumulated + static_cast<int>(!(house.looted));
		}
	);

	// TODO : Take into account full inventory
	return static_cast<float>(numUnlooted) * 10.0f; // The more houses that aren't looted, the higher the value
}

void SelectHouseAction::Execute(SurvivorMemory& memory)
{
	// TODO : Find best fitting house
	// Factors:
	// - Distance
	// - Time last seen
	// - Safety (if I somehow store that??)

	HouseMemory* pBestHouse = nullptr;
	double bestHouseDistance = FLT_MAX;

	for (auto& house : memory.houses) {
		if (house.looted) continue;

		const float distance = FVector::DistSquared(house.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation());

		if (distance < bestHouseDistance) {
			bestHouseDistance = distance;
			pBestHouse = &house;
		}
	}

	memory.pSelectedHouse = pBestHouse;
}

// ENTER HOUSE
EnterHouseAction::EnterHouseAction()
{
	m_pBehavior = MakeUnique<PathFollow>();

	UE_LOG(LogTemp, Log, TEXT("Enter House Action Created"));
}

float EnterHouseAction::Evaluate(const SurvivorMemory& memory)
{
	if (!memory.pSelectedHouse) return 0.0f; // no unlooted house within memory

	const auto& pInv = memory.pInventory;
	const auto& pHP = memory.pHealth;
	const auto& pStam = memory.pStamina;

	float value{};

	const int numFreeSlots = SurvivorUtils::GetNumberOfFreeSlots(pInv);

	value += static_cast<float>(numFreeSlots) * 2.0f;
	value += (1.0f - SurvivorUtils::GetHealthPercent(pHP)) * 5.0f;
	value += (1.0f - SurvivorUtils::GetStaminaPercent(pStam)) * 2.0f;

	// Stuff to modify value:
	// If there are nearby houses
	// How full inventory is
	// HP / Stamina state
	// If being chased?

	return value;
}

void EnterHouseAction::Execute(SurvivorMemory& memory)
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

// LOOT ENTERED HOUSE 