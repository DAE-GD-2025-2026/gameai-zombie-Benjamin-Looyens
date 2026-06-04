#pragma once
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>

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

	float GetHealthPercent(UHealthComponent* pHealth)
	{
		if (!pHealth) return 0.0f;

		return static_cast<float>(pHealth->GetHealth()) / static_cast<float>(pHealth->GetMaxHealth());
	}

	float GetStaminaPercent(UStaminaComponent* pStamina)
	{
		if (!pStamina) return 0.0f;

		return static_cast<float>(pStamina->GetCurrentStamina()) / static_cast<float>(pStamina->GetMaxStamina());
	}
}

//ASurvivorPawn
//UInventoryComponent
//UHealthComponent
//UStaminaComponent