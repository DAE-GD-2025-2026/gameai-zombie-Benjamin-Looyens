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
	if (memory.items.Num() <= 0) return 0.0f; // No known items

	//if (memory.items_medkits.Num() <= 0 &&
	//	memory.items_weapons.Num() <= 0 &&
	//	memory.items_food.Num() <= 0) 
	//	return 0.0f;

	m_PickupableIndex = -1;
	
	const auto& pInv = memory.pInventory;
	const int freeSlots = SurvivorUtils::GetNumberOfFreeSlots(pInv);

	if (freeSlots <= 0) return 0.0f; // No free item slots (maybe I can do some logic to switch out items though)

	const auto& items = memory.items;
	const float pickupRange = pInv->GetPickupRange();

	// TODO : some logic on deciding if interested in the pickup

	for (const auto& item : items) {
		m_PickupableIndex++;
		if (FVector::DistSquared(item.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation()) < (pickupRange * pickupRange) && 
			!SurvivorUtils::InventoryContains(pInv, item.ptr->GetItemType())) {
			return 35.0f; // TODO : Put some thought into what value this should be
		}
	}

	m_PickupableIndex = -1;

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

	auto& item = memory.items[m_PickupableIndex]; // It has crashed here before, but only once and I was not able to reproduce it at all?

	switch (item.ptr->GetItemType()) {
	case EItemType::Garbage:
		if (!item.ptr->Destroy()) UE_LOG(LogTemp, Log, TEXT("CANT DESTROY!"));
		break;
	default:
		pInv->GrabItem(freeSlot, item.ptr);
	}

	// TODO : Change destorying Garbage to it's own action
}

void CollectItemAction::LateExecute(SurvivorMemory& memory)
{
	memory.items.RemoveAt(m_PickupableIndex);

	m_PickupableIndex = -1;
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
