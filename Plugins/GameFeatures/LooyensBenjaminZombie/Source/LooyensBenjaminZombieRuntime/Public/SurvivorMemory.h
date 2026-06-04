#pragma once

class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;
class ASurvivorPawn;
class AHouse;


struct HouseMemory
{
	HouseMemory(AHouse* pHouse, double timeNow)
		: ptr{ pHouse }, lastSeen{ timeNow }
	{};

	AHouse* ptr;
	double lastSeen;
	bool looted{ false };
};

struct ZombieMemory
{
	// TODO : Zombie Memory
};

struct PurgeMemory
{
	// TODO : Purge Zones
};

struct SurvivorMemory
{
	// Add Memory of:
	// - Visible Zombies, Houses and Items
	// - Last seen Houses and Items
	// - Purge Zones

	// Pointers to Survivor's Components
	ASurvivorPawn* pSurvivor = nullptr;
	UInventoryComponent* pInventory = nullptr;
	UHealthComponent* pHealth = nullptr;
	UStaminaComponent* pStamina = nullptr;

	// Active Memory
	TArray<HouseMemory> houses{};
	HouseMemory* pSelectedHouse = nullptr;

	TArray<ZombieMemory> zombies{};
	TArray<PurgeMemory> purgeZones{};
};