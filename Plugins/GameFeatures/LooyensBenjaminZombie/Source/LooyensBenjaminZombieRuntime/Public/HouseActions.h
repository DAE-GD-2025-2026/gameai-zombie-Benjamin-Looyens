#pragma once
#include "SurvivorUtilityAction.h"
#include "SteeringBehaviors.h"

// GENERAL:
// HACK : A lot of Actions use a steering behavior at the end of them, maybe I should just make a steering action and call the base class Execute for these

struct HouseMemory;

class SelectHouseAction : public ISurvivorUtilityAction
{
public:
	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;
};

class EnterHouseAction : public ISurvivorUtilityAction
{
public:
	EnterHouseAction();

	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;

private:
	TUniquePtr<PathFollow> m_pBehavior{};

	HouseMemory* m_pLatestHouse = nullptr;
};

class ExitHouseAction : public ISurvivorUtilityAction
{
public:
	ExitHouseAction();

	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;
	virtual void LateExecute(SurvivorMemory& memory) override;

private:
	TUniquePtr<PathFollow> m_pBehavior{};

	HouseMemory* m_pLatestHouse = nullptr;

	// Leave house action:
	// Check if survivor is within bounds of a house that has been visited, if so then yes
};

class LootHouseAction : public ISurvivorUtilityAction
{
public:
	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;

private:
	TUniquePtr<Seek> m_pBehavior{};
};


// Scan for Items
// Collect Item
// Idk where from here lol