#include "SurvivorUtils.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>
#include <GameAI_Zombie/Village/House/House.h>
#include <GameAI_Zombie/Zombies/BaseZombie.h>
#include "SurvivorMemory.h"

namespace SurvivorUtils {
	int GetNumberOfFreeSlots(UInventoryComponent* pInventory)
	{
		if (pInventory) {
			const auto& pItems = pInventory->GetInventory();

			return Algo::Accumulate(pItems, 0,
				[](int accumulated, const ABaseItem* pItem) {
					return accumulated + static_cast<int>(pItem == nullptr);
				}
			);
		}

		return 0;
	}

	int GetNumberOfOwnedItems(UInventoryComponent* pInventory)
	{
		if (pInventory) return pInventory->GetInventoryCapacity() - GetNumberOfFreeSlots(pInventory);

		return 0;
	}

	int GetMissingHealth(UHealthComponent* pHealth)
	{
		return pHealth->GetMaxHealth() - pHealth->GetHealth();
	}

	float GetHealthPercent(UHealthComponent* pHealth)
	{
		if (!pHealth) return 0.0f;

		return static_cast<float>(pHealth->GetHealth()) / static_cast<float>(pHealth->GetMaxHealth());
	}

	float GetMissingStamina(UStaminaComponent* pStamina)
	{
		if (!pStamina) return 0.0f;

		return pStamina->GetMaxStamina() - pStamina->GetCurrentStamina();
	}

	float GetStaminaPercent(UStaminaComponent* pStamina)
	{
		if (!pStamina) return 0.0f;

		return static_cast<float>(pStamina->GetCurrentStamina()) / static_cast<float>(pStamina->GetMaxStamina());
	}

	FBox HouseBoundsToBox(const FHouseBounds& houseBounds)
	{
		return {
			houseBounds.Origin - houseBounds.Extent,
			houseBounds.Origin + houseBounds.Extent
		};
	}

	bool IsSurvivorWithinHouse(ASurvivorPawn* pSurvivor, const FHouseBounds& houseBounds)
	{
		if (!pSurvivor) return false;

		const FBox houseBox = SurvivorUtils::HouseBoundsToBox(houseBounds);
		return houseBox.IsInside(pSurvivor->GetActorLocation());
	}

	std::pair<int, int> GetNumNearbyZombies(const SurvivorMemory& memory, float maxDistance)
	{
		const auto& zombies = memory.zombies;

		std::pair<int, int> retPair{};

		// I could probably just do an Accumulate here
		double closestDist{ DBL_MAX };
		for (int index{}; index < zombies.Num(); index++) {
			const auto& zombie = zombies[index];
			const double distance = FVector::DistSquared(zombie.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation());

			// TODO : Differentiate pistol & shotgun
			if (distance <= (maxDistance * maxDistance)) {
				// Within shooting range
				retPair.first++;

				if (distance < closestDist) {
					closestDist = distance;
					retPair.second = index;
				}
			}
		}

		return retPair;
	}
}