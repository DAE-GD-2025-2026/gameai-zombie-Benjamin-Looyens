#pragma once

class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;
class ASurvivorPawn;
struct FHouseBounds;

namespace SurvivorUtils {
	int GetNumberOfFreeSlots(UInventoryComponent* pInventory);
	int GetNumberOfOwnedItems(UInventoryComponent* pInventory);
	int GetMissingHealth(UHealthComponent* pHealth);
	float GetHealthPercent(UHealthComponent* pHealth);
	float GetMissingStamina(UStaminaComponent* pStamina);
	float GetStaminaPercent(UStaminaComponent* pStamina);
	FBox HouseBoundsToBox(const FHouseBounds& houseBounds);
	bool IsSurvivorWithinHouse(ASurvivorPawn* pSurvivor, const FHouseBounds& houseBounds);
}

//ASurvivorPawn
//UInventoryComponent
//UHealthComponent
//UStaminaComponent