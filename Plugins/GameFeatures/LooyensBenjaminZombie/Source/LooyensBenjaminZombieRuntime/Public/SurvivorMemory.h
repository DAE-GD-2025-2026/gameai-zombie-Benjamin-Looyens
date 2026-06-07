#pragma once

#include "Runtime/Core/Public/Math/Vector.h"

class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;
class ASurvivorPawn;
class AHouse;
class ABaseZombie;
class APurgeZone;
class ABaseItem;

struct HouseMemory
{
	HouseMemory(AHouse* pHouse, double timeNow)
		: ptr{ pHouse }, lastSeen{ timeNow }
	{};

	AHouse* ptr;
	double lastSeen;
	bool explored{ false };
	double timeSinceExplored{ }; // Alternatively, could store "time since explored", and only add the deltaTime to those with the explored bool as true
};

struct ZombieMemory
{
	// TODO : Zombie Memory
	ZombieMemory(ABaseZombie* pZombie, double timeNow, FVector curPos, FVector curVel)
		: ptr{ pZombie }, lastSeen{ timeNow }, lastSeenPos{ curPos }, lastSeenVelocity{ curVel }
	{};
	
	ABaseZombie* ptr;
	double lastSeen;
	// Should only base logic off the last position & velocity
	FVector lastSeenPos; 
	FVector lastSeenVelocity;
};

struct PurgeMemory
{
	// TODO : Purge Zones
	PurgeMemory(APurgeZone* pPurgeZone, double creationTime)
		: ptr{ pPurgeZone }, timeCreated{ creationTime }
	{};

	APurgeZone* ptr;
	double timeCreated;

	// HACK : Feels really wrong to just hard code it like this but there seems to be no way to access this data dynamically?
	static constexpr double s_PURGE_TIMER = 5.0f;
	static constexpr float s_PURGE_DIAMETER = 100.0f;
};

struct ItemMemory
{
	ItemMemory(ABaseItem* pItem, double timeNow)
		: ptr{ pItem }, lastSeen{ timeNow }
	{};

	ABaseItem* ptr;
	double lastSeen;
	HouseMemory* pContainingHouse = nullptr;
};

struct InventoryState
{
	bool hasMedkit{};
	bool hasWeapon{};
	bool hasFood{};
};

struct SurvivorMemory
{
	// Pointers to Survivor's Components
	ASurvivorPawn* pSurvivor = nullptr;
	UInventoryComponent* pInventory = nullptr;
	UHealthComponent* pHealth = nullptr;
	UStaminaComponent* pStamina = nullptr;

	// Active Memory
	TArray<HouseMemory> houses{};
	HouseMemory* pSelectedHouse = nullptr;
	float timeSpentInHouse{};
	static constexpr float s_MAX_TIME_SPENT_IN_HOUSE = 9.0f;

	TArray<ZombieMemory> zombies{};
	int numNearbyZombies = 0;
	int closestZombieIndex = -1;

	TArray<PurgeMemory> purgeZones{};

	TArray<ItemMemory> items_weapons{};
	TArray<ItemMemory> items_medkits{};
	TArray<ItemMemory> items_food{};
	TArray<ItemMemory> items_garbage{};

	TArray<FVector> exploredLocations{};
	bool explorePathDirty = false;

	InventoryState invState{};
};