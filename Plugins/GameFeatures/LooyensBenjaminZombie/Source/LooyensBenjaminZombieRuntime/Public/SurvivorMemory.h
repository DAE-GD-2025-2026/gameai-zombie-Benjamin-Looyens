#pragma once

#include "Runtime/Core/Public/Math/Vector.h"

class UInventoryComponent;
class UHealthComponent;
class UStaminaComponent;
class ASurvivorPawn;
class AHouse;
class ABaseZombie;
class APurgeZone;

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