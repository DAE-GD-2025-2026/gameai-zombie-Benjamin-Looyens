#pragma once

class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;
class ASurvivorPawn;
struct FHouseBounds;
struct SurvivorMemory;
enum class EItemType : uint8;

namespace SurvivorUtils {
	int GetNumberOfFreeSlots(UInventoryComponent* pInventory);
	int GetNumberOfOwnedItems(UInventoryComponent* pInventory);
	bool InventoryContains(UInventoryComponent* pInventory, const EItemType& type);

	int GetMissingHealth(UHealthComponent* pHealth);
	float GetHealthPercent(UHealthComponent* pHealth);
	
	float GetMissingStamina(UStaminaComponent* pStamina);
	float GetStaminaPercent(UStaminaComponent* pStamina);
	
	FBox HouseBoundsToBox(const FHouseBounds& houseBounds);
	bool IsSurvivorWithinHouse(ASurvivorPawn* pSurvivor, const FHouseBounds& houseBounds);

	// pair.first = *number zombies around*, pair.second = *index to closest zombie*
	std::pair<int, int> GetNumNearbyZombies(const SurvivorMemory& memory, float maxDistance);
}