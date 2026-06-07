#include "ItemActions.h"
#include "SurvivorDecisionMaker.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Village/House/House.h>
#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>
#include "SurvivorUtils.h"

// PICKUP
float CollectItemAction::Evaluate(const SurvivorMemory& memory)
{
	if (memory.items_medkits.Num() <= 0 &&
		memory.items_weapons.Num() <= 0 &&
		memory.items_food.Num() <= 0)
		return 0.0f; // No known items

	m_PickupableIndex = -1;

	const auto& pInv = memory.pInventory;
	const int freeSlots = SurvivorUtils::GetNumberOfFreeSlots(pInv);
	const float pickupRange = pInv->GetPickupRange();

	if (freeSlots <= 0) return 0.0f; // No free item slots (maybe I can do some logic to switch out items though)

	m_PickupableIndex = -1;
	if (!memory.invState.hasWeapon || freeSlots > 2) {
		const auto& weapons = memory.items_weapons;

		for (const auto& weapon : weapons) {
			m_PickupableIndex++;

			if (FVector::DistSquared(weapon.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation()) < (pickupRange * pickupRange)) {

				m_PickupableType = weapon.ptr->GetItemType();
				return 35.0f; // TODO : Put some thought into what value this should be
			}
		}
	}

	m_PickupableIndex = -1;
	if (!memory.invState.hasMedkit || freeSlots > 2) {
		const auto& medkits = memory.items_medkits;

		for (const auto& medkit : medkits) {
			m_PickupableIndex++;

			if (FVector::DistSquared(medkit.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation()) < (pickupRange * pickupRange)) {

				m_PickupableType = EItemType::Medkit;
				return 35.0f; // TODO : Put some thought into what value this should be
			}
		}
	}

	m_PickupableIndex = -1;
	if (!memory.invState.hasFood || freeSlots > 2) {
		const auto& foodList = memory.items_food;

		for (const auto& food : foodList) {
			m_PickupableIndex++;

			if (FVector::DistSquared(food.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation()) < (pickupRange * pickupRange)) {

				m_PickupableType = EItemType::Food;
				return 35.0f; // TODO : Put some thought into what value this should be
			}
		}
	}

	return 0.0f;
}

void CollectItemAction::Execute(SurvivorMemory& memory)
{
	if (m_PickupableIndex == -1) return;

	const auto& pInv = memory.pInventory;
	const auto& ownedItems = pInv->GetInventory();
	
	int freeSlot{};
	for (const auto& pItem : ownedItems) {
		if (pItem) freeSlot++;
		else break;
	}

	UE_LOG(LogTemp, Log, TEXT("Free slot found at [%i]"), freeSlot);

	switch (m_PickupableType) {
	case EItemType::Food:
		pInv->GrabItem(freeSlot, memory.items_food[m_PickupableIndex].ptr);
		memory.items_food[m_PickupableIndex].pContainingHouse = nullptr;
		memory.items_food.RemoveAt(m_PickupableIndex);
		break;
	case EItemType::Medkit:
		pInv->GrabItem(freeSlot, memory.items_medkits[m_PickupableIndex].ptr);
		memory.items_medkits[m_PickupableIndex].pContainingHouse = nullptr;
		memory.items_medkits.RemoveAt(m_PickupableIndex);
		break;
	case EItemType::Shotgun:
	case EItemType::Pistol:
		pInv->GrabItem(freeSlot, memory.items_weapons[m_PickupableIndex].ptr);
		memory.items_weapons[m_PickupableIndex].pContainingHouse = nullptr;
		memory.items_weapons.RemoveAt(m_PickupableIndex);
		break;
	}
}

void CollectItemAction::LateExecute(SurvivorMemory& memory)
{
	m_PickupableIndex = -1;
	m_PickupableType = EItemType::Garbage;
}

// DESTROY
float DestroyGarbageAction::Evaluate(const SurvivorMemory& memory)
{
	const float pickupRange = memory.pInventory->GetPickupRange();
	const auto& garbages = memory.items_garbage;

	m_DestroyableIndex = -1;

	for (const auto& garbage : garbages) {
		m_DestroyableIndex++;

		if (FVector::DistSquared(garbage.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation()) < (pickupRange * pickupRange)) {
			return 36.0f; // TODO : Put some thought into this value
		}
	}

	return 0.0f;
}

void DestroyGarbageAction::Execute(SurvivorMemory& memory)
{
	auto& item = memory.items_garbage[m_DestroyableIndex];
	if (!item.ptr->Destroy()) UE_LOG(LogTemp, Log, TEXT("CANT DESTROY!"));
	memory.items_garbage.RemoveAt(m_DestroyableIndex);
}

void DestroyGarbageAction::LateExecute(SurvivorMemory& memory)
{
	m_DestroyableIndex = -1;
}

// HEAL
float HealAction::Evaluate(const SurvivorMemory& memory)
{
	// Check remaining HP
	// Compare to Medkits in inventory (if have any)
	// if medkit value >= missing hp store index to that medkit
	// Use medkit through index
	// Destroy medkit through index
	// Set inventory slot as nullptr?

	const auto& pInv = memory.pInventory;
	const auto& pHP = memory.pHealth;
	const auto& pItems = pInv->GetInventory();
	const int missingHP = SurvivorUtils::GetMissingHealth(pHP);;
	const int curHP = pHP->GetHealth();
	const int maxHP = pHP->GetMaxHealth();

	if (missingHP == 0) return 0.0f;

	float value{ static_cast<float>(missingHP) };

	std::pair<int, int> bestMedKit{ 0, -1 }; // first: index, second: heal value
	for (int index{}; index < pItems.Num(); index++) {
		const auto& pItem = pItems[index];
		
		if (!pItem) continue;

		switch (pItem->GetItemType()) {
		case EItemType::Medkit:
		{
			const int healVal = pItem->GetValue();
			if (healVal > bestMedKit.second) {
				bestMedKit.first = index;
				bestMedKit.second = healVal;

				if (curHP + healVal == maxHP) index = pItems.Num(); // artificial "break" since I'm in a switch case
			}
			break;
		}
		}
	}

	if (bestMedKit.second == -1) return 0.0f; // No medkits found

	const int healVal = curHP + bestMedKit.second; // the higher the filled HP is, the better
	value += static_cast<float>(healVal);
	if (value > 10.0f) value -= (healVal - static_cast<float>(maxHP)) * 5.0f; // deduct points for overflowing hp

	m_HealableIndex = bestMedKit.first;

	return value;
}

void HealAction::Execute(SurvivorMemory& memory)
{
	memory.pInventory->UseItem(m_HealableIndex);
	memory.pInventory->RemoveItem(m_HealableIndex);
}

void HealAction::LateExecute(SurvivorMemory& memory)
{
	m_HealableIndex = -1;
}

float EatAction::Evaluate(const SurvivorMemory& memory)
{
	const auto& pInv = memory.pInventory;
	const auto& pStamina = memory.pStamina;
	const auto& pItems = pInv->GetInventory();
	const float missingStamina = SurvivorUtils::GetMissingStamina(pStamina);;
	const float curStamina = pStamina->GetCurrentStamina();
	const float maxStamina = pStamina->GetMaxStamina();

	if (missingStamina < 1.0f) return 0.0f;

	float value{ missingStamina };

	std::pair<int, int> bestFood{ 0, -1 }; // first: index, second: heal value
	for (int index{}; index < pItems.Num(); index++) {
		const auto& pItem = pItems[index];

		if (!pItem) continue;

		switch (pItem->GetItemType()) {
		case EItemType::Food:
		{
			// TODO : Fix Medkit & Food Logic
			// Fix logic with the fact that higher isnt actually better
			// If the food has less overflow, it is also much better

			const int foodVal = pItem->GetValue();
			if (foodVal > bestFood.second) {
				bestFood.first = index;
				bestFood.second = foodVal;

				const float newStamina = curStamina + static_cast<float>(foodVal);
				if (newStamina > (maxStamina - 1.0f) &&
					newStamina < (maxStamina + 1.0f)) index = pItems.Num(); // artificial "break" since I'm in a switch case
			}
			break;
		}
		}
	}

	if (bestFood.second == -1) return 0.0f; // No medkits found

	const int foodVal = static_cast<int>(curStamina) + bestFood.second; // the higher the filled stamina is, the better
	value += static_cast<float>(foodVal);
	if (static_cast<float>(bestFood.second) + curStamina > maxStamina) value -= foodVal * 2.0f; // deduct points for overflowing stamina

	m_EatableIndex = bestFood.first;

	return value;
}

void EatAction::Execute(SurvivorMemory& memory)
{
	memory.pInventory->UseItem(m_EatableIndex);
	memory.pInventory->RemoveItem(m_EatableIndex);
}

void EatAction::LateExecute(SurvivorMemory& memory)
{
	m_EatableIndex = -1;
}
