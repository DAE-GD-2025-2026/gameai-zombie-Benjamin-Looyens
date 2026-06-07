// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorDecisionMaker.h"

#include <GameAI_Zombie/Common/HealthComponent.h>
#include <GameAI_Zombie/Common/InventoryComponent.h>
#include <GameAI_Zombie/Common/StaminaComponent.h>
#include <GameAI_Zombie/Zombies/BaseZombie.h>
#include <GameAI_Zombie/Village/House/House.h>
#include <GameAI_Zombie/PurgeZones/PurgeZone.h>

#include "WanderAction.h"
#include "HouseActions.h"
#include "ItemActions.h"
#include "ZombieActions.h"
#include "SurvivorUtils.h"

// Null Action for Passives
class NullAction final : public ISurvivorUtilityAction
{
	virtual float Evaluate(const SurvivorMemory& memory) override { return 1.0f; };
	virtual void Execute(SurvivorMemory& memory) override {};
};

// Sets default values for this component's properties
USurvivorDecisionMaker::USurvivorDecisionMaker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

// HACK : Separate Init Function is weird but BeginPlay() would never be called
void USurvivorDecisionMaker::Init()
{
	// Get Survivor Components
	m_Memory.pSurvivor = Cast<ASurvivorPawn>(GetOwner());
	m_Memory.pInventory = GetOwner()->GetComponentByClass<UInventoryComponent>();
	m_Memory.pHealth = GetOwner()->GetComponentByClass<UHealthComponent>();
	m_Memory.pStamina = GetOwner()->GetComponentByClass<UStaminaComponent>();

	// Create Utility Actions
	m_Actions.Add(MakeUnique<WanderAction>());
	m_Actions.Add(MakeUnique<SelectHouseAction>());
	m_Actions.Add(MakeUnique<EnterHouseAction>());
	m_Actions.Add(MakeUnique<ExitHouseAction>());
	m_Actions.Add(MakeUnique<ExploreHouseAction>());
	m_Actions.Add(MakeUnique<CollectItemAction>());
	m_Actions.Add(MakeUnique<ShootZombieAction>());

	m_PassiveActions.Add(MakeUnique<NullAction>());
	m_PassiveActions.Add(MakeUnique<HealAction>());
	m_PassiveActions.Add(MakeUnique<EatAction>());
	m_PassiveActions.Add(MakeUnique<RunAction>());

	// Goals & Actions to Add:
	// - [GOAL] Search For Items
	//		- [ACTION] Look for house
	//		- [ACTION] Enter house
	//		- [ACTION] Loot house
	//		- [ACTION] Leave house
	// - [GOAL] Defend self from zombie
	//		- [ACTION] Search inventory for weapon
	//			- [ACTION] Flee from zombie
	//			- [GOAL] Search for Items ^
	//		- [ACTION] Face zombie
	//		- [ACTION] Use weapon
	// - [GOAL] Heal from damage
	//		- [ACTION] Search inventory for medkit
	//			- [GOAL] Search for Items ^ 
	//		- [ACTION] Use Medkit
	// - [GOAL] Recover Stamina
	//		- [ACTION] Search inventory for food
	//			- [GOAL] Search for Items ^ 
	//		- [ACTION] Use food
}

// Called when the game starts
void USurvivorDecisionMaker::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void USurvivorDecisionMaker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_Actions.Num() <= 0) return; // No actions to loop through

	// Potentially, I could split up the actions into different types:
	// - "Thinking Actions", where it calculates options (such as best item, best zombie to attack, etc)
	// - "Movement Actions", where it calculates and applies the steering/movement necessary
	// - "Item Actions", where it uses the items that were decided (to attack or heal, etc)

	// Logic after movement for previous action
	if (m_PrevAction != -1) m_Actions[m_PrevAction]->LateExecute(m_Memory); 

	// Update Old Memory
	const double curTime = GetWorld()->GetTimeSeconds();

	m_Memory.purgeZones.RemoveAll([&](const PurgeMemory& purge) {
		return curTime - purge.timeCreated >= PurgeMemory::s_PURGE_TIMER; // Purge Zone Should have expired
		});

	m_Memory.zombies.RemoveAll([&](const ZombieMemory& zombie) {
		if (!IsValid(zombie.ptr)) {
			UE_LOG(LogTemp, Log, TEXT("Removed Zombie!"));
			return true; // Zombie == nullptr
		}

		return false;
	});

	m_Memory.houses.RemoveAll([&](const HouseMemory& house) {
		if (!IsValid(house.ptr)) {
			UE_LOG(LogTemp, Log, TEXT("Removed House!"));
			return true; // house == nullptr (somehow)
		}

		return false;
	});
	// TODO : Clear out items that havent been seen in a while?

	// Calculate Current Memory
	constexpr float s_MAXIMUM_DISTANCE_AWAY_SHOTGUN = 300.0f;
	const auto zombies = SurvivorUtils::GetNumNearbyZombies(m_Memory, s_MAXIMUM_DISTANCE_AWAY_SHOTGUN);
	m_Memory.numNearbyZombies = zombies.first;
	if (zombies.second != -1) m_Memory.closestZombieIndex = zombies.second;


	if (m_PrevPassive != -1) m_PassiveActions[m_PrevPassive]->LateExecute(m_Memory); // HACK : Order of this execution is real silly, but for the way I set it up this works

	// Calculate Utility Scores
	constexpr int DEBUG_MESSAGE_OFFSET = 6;
	std::pair<int, float> bestAction{ 0, -50.0f };
	for (size_t index{}; index < m_Actions.Num(); index++) {
		const auto& pCurAction = m_Actions[index];
		const float curValue = pCurAction->Evaluate(m_Memory);

		if (curValue > bestAction.second) {
			bestAction.first = index;
			bestAction.second = curValue;
		}

		GEngine->AddOnScreenDebugMessage(DEBUG_MESSAGE_OFFSET + index, 2.5f, FColor::Cyan, FString::Printf(TEXT("[%i] Score: %f"), index, curValue));
	}

	std::pair<int, float> bestPassive{ 0, -50.0f };
	for (size_t index{}; index < m_PassiveActions.Num(); index++) {
		const auto& pCurAction = m_PassiveActions[index];
		const float curValue = pCurAction->Evaluate(m_Memory);

		if (curValue > bestPassive.second) {
			bestPassive.first = index;
			bestPassive.second = curValue;
		}

		GEngine->AddOnScreenDebugMessage(static_cast<int>(m_Actions.Num()) + DEBUG_MESSAGE_OFFSET + index, 2.5f, FColor::Blue, FString::Printf(TEXT("[%i] Score: %f"), index, curValue));
	}

	// Execute Utility Actions
	m_PassiveActions[bestPassive.first]->Execute(m_Memory);
	m_PrevPassive = bestPassive.first;

	m_Actions[bestAction.first]->Execute(m_Memory);
	m_PrevAction = bestAction.first;

	// Maybe store steering behaviors elsewhere (in memory?)
	// As they could potentially be better off being reused sometimes?
	// Depends on the behavior specifically though, as some probably prefer to keep their internal storage
}

void USurvivorDecisionMaker::AddHouseMemory(AHouse* pHouse)
{
	auto& houses = m_Memory.houses;
	for (const auto& house : houses) {
		if (house.ptr == pHouse) return; // House already known
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("New House added to memory index [%i]"), houses.Num()));
	houses.Add({ pHouse, GetWorld()->GetTimeSeconds() });
}

void USurvivorDecisionMaker::AddPurgeMemory(APurgeZone* pPurgeZone)
{
	auto& zones = m_Memory.purgeZones;
	for (const auto& zone : zones) {
		if (zone.ptr == pPurgeZone) return; // Purge zone already known
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("New Purge Zone added to memory index [%i]"), zones.Num()));
	zones.Add({ pPurgeZone, pPurgeZone->CreationTime });
}

void USurvivorDecisionMaker::AddZombieMemory(ABaseZombie* pZombie)
{
	auto& zombies = m_Memory.zombies;
	for (auto& zombie : zombies) {
		if (zombie.ptr == pZombie) {
			zombie.lastSeen = GetWorld()->GetTimeSeconds();
			zombie.lastSeenPos = pZombie->GetActorLocation();
			zombie.lastSeenVelocity = pZombie->GetVelocity();
			
			return; // Zombie already known
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("New zombie added to memory index [%i]"), zombies.Num()));
	zombies.Add({ pZombie, GetWorld()->GetTimeSeconds(), pZombie->GetActorLocation(), pZombie->GetVelocity() });
}

void USurvivorDecisionMaker::AddItemMemory(ABaseItem* pItem)
{
	auto& items = m_Memory.items;
	for (auto& item : items) {
		if (item.ptr == pItem) {
			item.lastSeen = GetWorld()->GetTimeSeconds();

			return; // Item already known
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("New item added to memory index [%i]"), items.Num()));
	items.Add({ pItem, GetWorld()->GetTimeSeconds() });
}

