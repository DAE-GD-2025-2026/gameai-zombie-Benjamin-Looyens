#include "ZombieActions.h"
#include "SurvivorDecisionMaker.h"
#include <GameAI_Zombie/Survivor/SurvivorPawn.h>
#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>
#include <GameAI_Zombie/Zombies/BaseZombie.h>
#include <GameAI_Zombie/Village/House/House.h>
#include "SurvivorUtils.h"

// SHOOT
float ShootZombieAction::Evaluate(const SurvivorMemory& memory)
{
	if (memory.zombies.IsEmpty()) return 0.0f;

	// Decide best weapon based on:
	// - HOW many zombies are nearby
	// - Distance from self (further away, dont use shotguns)

	const auto& pInv = memory.pInventory;
	const auto& items = pInv->GetInventory();
	const auto& zombies = memory.zombies;

	// Loop over zombies
	// Find the amount of them within range
	// If more than 2, prioritise shotgun

	const int& withinRange = memory.numNearbyZombies;
	if (withinRange == 0) return 0.0f;
	if (memory.closestZombieIndex == -1) return 0.0f;

	// Raycast to see if possible to hit zombie
	FCollisionQueryParams rayParams;
	rayParams.AddIgnoredActor(memory.pSurvivor);
	FHitResult rayHit;
	bool didHit = memory.pSurvivor->GetWorld()->LineTraceSingleByChannel(
		rayHit,
		memory.pSurvivor->GetActorLocation(), zombies[memory.closestZombieIndex].ptr->GetActorLocation(),
		ECC_Visibility, rayParams);

	if (didHit && Cast<AHouse>(rayHit.GetActor())) return 0.0f; // Hit a house, thus cant hit zombie

	GEngine->AddOnScreenDebugMessage(20, 2.5f, FColor::Red, FString::Printf(TEXT("%i Zombies in range"), withinRange));

	// Maybe try differentiate between weapons with low & high ammo
	for (int index{}; index < items.Num(); index++) {
		const auto& pItem = items[index];
		
		if (!pItem) continue;

		// this feels kinda silly to do but it makes sense?
		if (m_BestWeaponIndex == -1) {
			switch (pItem->GetItemType()) {
			case EItemType::Shotgun:
			case EItemType::Pistol:
				m_BestWeaponIndex = index;
				break;
			}
		}
		else {
			// TODO : Differentiate between low and high ammo here
			switch (pItem->GetItemType()) {
			case EItemType::Shotgun:
				if (withinRange > 1) m_BestWeaponIndex = index;
				// TODO : Calculate if they are near each other
				break;
			case EItemType::Pistol:
				m_BestWeaponIndex = index;
				break;
			}
		}
	}

	if (m_BestWeaponIndex == -1) return 0.0f;

	// Calculate value uhhhhh
	// Raycast and see if it hits a wall, then dont

	return 50.0f;
}

void ShootZombieAction::Execute(SurvivorMemory& memory)
{
	if (m_BestWeaponIndex == -1 || memory.closestZombieIndex == -1) return;

	auto& pSurvivor = memory.pSurvivor;
	auto& closeZombie = memory.zombies[memory.closestZombieIndex];

	const FVector toZombie = closeZombie.ptr->GetActorLocation() - pSurvivor->GetActorLocation();
	const FVector toZombieXY{ toZombie.X, toZombie.Y, 0 };
	pSurvivor->SetActorRotation(toZombieXY.ToOrientationRotator());
}

void ShootZombieAction::LateExecute(SurvivorMemory& memory)
{
	// Shoot here, since it is after physics calculations?

	auto& pInv = memory.pInventory;

	UE_LOG(LogTemp, Log, TEXT("Tried to shoot zombie at memory index [%i] with item at index [%i]"), memory.closestZombieIndex, m_BestWeaponIndex);

	pInv->UseItem(m_BestWeaponIndex);

	if (pInv->GetInventory()[m_BestWeaponIndex]->GetValue() <= 0) pInv->RemoveItem(m_BestWeaponIndex); // Toss weapon with no ammo


	m_BestWeaponIndex = -1;
	memory.closestZombieIndex = -1;
}

// RUN
float RunAction::Evaluate(const SurvivorMemory& memory)
{
	if (memory.numNearbyZombies > 0) return 5.0f;

	return 0.0f;
}

void RunAction::Execute(SurvivorMemory& memory)
{
	if (!memory.pSurvivor->IsRunning()) UE_LOG(LogTemp, Log, TEXT("Started running!"));

	memory.pSurvivor->StartRunning();
}

void RunAction::LateExecute(SurvivorMemory& memory)
{
	if (memory.numNearbyZombies <= 0) {
		UE_LOG(LogTemp, Log, TEXT("Stopped running!"));
		memory.pSurvivor->StopRunning();
	}
}
