#include "ItemActions.h"
#include "SurvivorDecisionMaker.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Village/House/House.h>
#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>
#include "SurvivorUtils.h"

float CollectItemAction::Evaluate(const SurvivorMemory& memory)
{
	if (memory.items.Num() <= 0) return 0.0f; // No known items

	const auto& inv = memory.pInventory;
	const int freeSlots = SurvivorUtils::GetNumberOfFreeSlots(inv);

	if (freeSlots <= 0) return 0.0f; // No free item slots (maybe I can do some logic to switch out items though)

	const auto& items = memory.items;
	const float pickupRange = inv->GetPickupRange();

	// TODO : some logic on deciding if interested in the pickup

	for (const auto& item : items) {
		m_PickupableIndex++;
		if (FVector::Distance(item.ptr->GetActorLocation(), memory.pSurvivor->GetActorLocation()) < pickupRange) {
			return 35.0f; // TODO : Put some thought into what value this should be
		}
	}

	m_PickupableIndex = -1;

	return 0.0f;
}

void CollectItemAction::Execute(SurvivorMemory& memory)
{
	if (m_PickupableIndex == -1) return;

	const auto& inv = memory.pInventory;
	const auto& ownedItems = inv->GetInventory();

	int freeSlot{};
	for (const auto& item : ownedItems) {
		if (item) freeSlot++;
		else break;
	}

	UE_LOG(LogTemp, Log, TEXT("Free slot found at [%i]"), freeSlot);

	inv->GrabItem(freeSlot, memory.items[m_PickupableIndex].ptr);
}

void CollectItemAction::LateExecute(SurvivorMemory& memory)
{
	memory.items.RemoveAt(m_PickupableIndex);

	//memory.items.RemoveAll([&](const ItemMemory& item) {
	//	return item.ptr == m_pPickupable->ptr;
	//});
	
	m_PickupableIndex = -1;
}
