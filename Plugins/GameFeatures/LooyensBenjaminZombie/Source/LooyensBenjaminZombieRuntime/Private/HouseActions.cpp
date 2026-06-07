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

	int numUnexplored = Algo::Accumulate(houses, 0,
		[](int accumulated, const HouseMemory& house) {
			return accumulated + static_cast<int>(!(house.explored));
		}
	);

	// TODO : Highly prioritize if dont have a weapon

	// TODO : Take into account full inventory
	return static_cast<float>(numUnexplored) * 10.0f; // The more houses that aren't explored, the higher the value
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
		if (house.explored) continue;

		const float distance = FVector::DistSquared(house.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation());

		if (distance < bestHouseDistance) {
			bestHouseDistance = distance;
			pBestHouse = &house;
		}
	}

	memory.pSelectedHouse = pBestHouse;
	memory.timeSpentInHouse = 0.0f;
}

// ENTER HOUSE
EnterHouseAction::EnterHouseAction()
{
	m_pBehavior = MakeUnique<PathFollow>();

	UE_LOG(LogTemp, Log, TEXT("Enter House Action Created"));
}

float EnterHouseAction::Evaluate(const SurvivorMemory& memory)
{
	// TODO : EXIT HOUSE WHEN FULL INVENTORY
	// House is not marked as explored when attaining a full inventory
	// So will either wander or get stuck in the "enter house" state
	
	// Furthermore, later when inventory clears up the selecting house is messy


	if (!memory.pSelectedHouse) return 0.0f; // no unlooted house within memory

	const auto& pInv = memory.pInventory;
	const auto& pHP = memory.pHealth;
	const auto& pStam = memory.pStamina;

	float value{};

	//const int numFreeSlots = SurvivorUtils::GetNumberOfFreeSlots(pInv);
	//
	//value += static_cast<float>(numFreeSlots) * 2.0f;
	//value += (1.0f - SurvivorUtils::GetHealthPercent(pHP)) * 5.0f;
	//value += (1.0f - SurvivorUtils::GetStaminaPercent(pStam)) * 2.0f;

	// TEMP : Set value to 2.0f for testing
	value = 2.0f;

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

	if (m_pLatestHouse != memory.pSelectedHouse) {
		const auto path = memory.pSurvivor->CalculatePath(memory.pSelectedHouse->ptr->GetActorLocation());

		m_pBehavior->SetPath(path);

		m_pLatestHouse = memory.pSelectedHouse;
	}

	const auto steering = m_pBehavior->CalculateSteering(pWorld->GetDeltaSeconds(), *(memory.pSurvivor));
	ISteeringBehavior::ApplySteering(memory.pSurvivor, steering);
}

// EXIT ENTERED HOUSE
ExitHouseAction::ExitHouseAction()
{
	m_pBehavior = MakeUnique<PathFollow>();

	UE_LOG(LogTemp, Log, TEXT("Exit House Action Created"));
}

float ExitHouseAction::Evaluate(const SurvivorMemory& memory)
{
	if (!memory.pSelectedHouse) return 0.0f;

	float value{};

	// The longer we spend in the house, the more likely we want to leave
	// However, we first want to make sure it is explored
	//value += memory.timeSpentInHouse * static_cast<float>(SurvivorUtils::IsSurvivorWithinHouse(memory.pSurvivor, memory.pSelectedHouse->ptr->GetBounds())) * 3.0f; 
	//value += memory.timeSpentInHouse * static_cast<float>(memory.pSelectedHouse->explored); 

	if (memory.pSelectedHouse->explored) {
		//value += memory.timeSpentInHouse;
		value += 16.0f; // TODO : Make this more dynamic
	}


	return value;
}

void ExitHouseAction::Execute(SurvivorMemory& memory)
{
	const auto& pWorld = memory.pSurvivor->GetWorld();

	if (m_pLatestHouse != memory.pSelectedHouse) {
		const FHouseBounds bounds = memory.pSelectedHouse->ptr->GetBounds();
		const FVector outsideBounds = bounds.Origin + (bounds.Extent * 1.5);

		const auto path = memory.pSurvivor->CalculatePath(outsideBounds);

		m_pBehavior->SetPath(path);

		m_pLatestHouse = memory.pSelectedHouse;
	}

	const auto steering = m_pBehavior->CalculateSteering(pWorld->GetDeltaSeconds(), *(memory.pSurvivor));
	ISteeringBehavior::ApplySteering(memory.pSurvivor, steering);
}

void ExitHouseAction::LateExecute(SurvivorMemory& memory)
{
	if (!SurvivorUtils::IsSurvivorWithinHouse(memory.pSurvivor, memory.pSelectedHouse->ptr->GetBounds())) {
		memory.timeSpentInHouse = 0.0f;
		memory.pSelectedHouse->explored = true;
		memory.pSelectedHouse->lastExplored = memory.pSurvivor->GetWorld()->GetTimeSeconds();
		memory.pSelectedHouse = nullptr;
		m_pLatestHouse = nullptr;

		UE_LOG(LogTemp, Log, TEXT("Exited House"));

		memory.explorePathDirty = true; // Maybe just set this in the enter house execute (when calc path)
	}
}

// LOOT ENTERED HOUSE 
ExploreHouseAction::ExploreHouseAction()
{
	m_pBehavior = MakeUnique<PathFollow>();

	UE_LOG(LogTemp, Log, TEXT("Loot House Action Created"));
}

float ExploreHouseAction::Evaluate(const SurvivorMemory& memory)
{
	if (!memory.pSelectedHouse) return 0.0f;
	if (!IsValid(memory.pSelectedHouse->ptr)) return 0.0f; // Had an instance where it was nullptr, but cant say I know why

	const auto& pInv = memory.pInventory;
	
	float value{};

	if (SurvivorUtils::IsSurvivorWithinHouse(memory.pSurvivor, memory.pSelectedHouse->ptr->GetBounds())) {
		//const int numFreeSlots = SurvivorUtils::GetNumberOfFreeSlots(pInv);
		//value += static_cast<float>(numFreeSlots) * 5.0f;

		return 3.0f; // TEMP : Return flat value for testing
	}

	return value;
}

void ExploreHouseAction::Execute(SurvivorMemory& memory)
{
	const float deltaTime = memory.pSurvivor->GetWorld()->GetDeltaSeconds();

	if (m_pLatestHouse != memory.pSelectedHouse) {
		m_pLatestHouse = memory.pSelectedHouse;
	
		const float agentRadius = memory.pSurvivor->GetSimpleCollisionRadius() * 4.15f;
		FHouseBounds bounds = m_pLatestHouse->ptr->GetBounds();
		bounds.Extent -= { agentRadius, agentRadius, 0.0f };

		const FBox houseBox = SurvivorUtils::HouseBoundsToBox(bounds);
		const double& z = houseBox.Min.Z;

		const FVector& survPos = memory.pSurvivor->GetActorLocation();

		const FVector bl = houseBox.Min;
		const FVector tl = { houseBox.Min.X, houseBox.Max.Y, z };
		const FVector tr = houseBox.Max;
		const FVector br = { houseBox.Max.X, houseBox.Min.Y, z };

		TArray<FVector> path{};
		path.SetNum(4);

		path[0] = bl;
		path[1] = tl;
		path[2] = tr;
		path[3] = br;

		// Calculate closest point, putting that one first in path
		std::pair<int, double> closestPointIndex{ 0, DBL_MAX };
		for (int index{}; index < path.Num(); index++) {
			const double curDistance = FVector::DistSquared(path[index], survPos);
			
			if (curDistance < closestPointIndex.second) {
				closestPointIndex.first = index;
				closestPointIndex.second = curDistance;
			}
		}

		Algo::Rotate(path, closestPointIndex.first);

		path.SetNum(8);
		path[4] = path[0]; // Back to start
		path[5] = path[2]; // Diagonal across (to check middle of house)
		path[6] = path[1]; // Back to second
		path[7] = path[3]; // Diagonal across (to check middle of house)

		m_pBehavior->SetPath(path);
	}

	SteeringOutput steering = m_pBehavior->CalculateSteering(deltaTime, *(memory.pSurvivor));
	steering.AngularVelocity = 360.0f * deltaTime; // 180 degree rotation per second

	ISteeringBehavior::ApplySteering(memory.pSurvivor, steering, false); // TODO : Disable auto orient, and rotate constantly so it can see all items

	//memory.timeSpentInHouse += deltaTime;
	//if (memory.timeSpentInHouse >= SurvivorMemory::s_MAX_TIME_SPENT_IN_HOUSE) {
	//	memory.pSelectedHouse->explored = true;
	//	memory.timeSpentInHouse += 1.0f; // Ensure it is above enough
	//}

	// TODO : Store Info on House
	// If inventory is full, instead of picking stuff up, store it in memory
	// To do this I still need to add a section to memory for this
	// And I need to be able to check what items are in fron of the survivor
}

void ExploreHouseAction::LateExecute(SurvivorMemory& memory)
{
	if (m_pBehavior->HasFinishedPath(*(memory.pSurvivor))) {
		memory.pSelectedHouse->explored = true;
	}
}
