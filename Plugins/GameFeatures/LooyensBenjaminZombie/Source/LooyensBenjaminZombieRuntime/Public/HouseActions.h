#pragma once
#include "SurvivorUtilityAction.h"
#include "SteeringBehaviors.h"

class SelectHouseAction : public ISurvivorUtilityAction
{
public:
	virtual float Evaluate(const SurvivorMemory& memory);
	virtual void Execute(SurvivorMemory& memory);
};

class EnterHouseAction : public ISurvivorUtilityAction
{
public:
	EnterHouseAction();

	virtual float Evaluate(const SurvivorMemory& memory);
	virtual void Execute(SurvivorMemory& memory);

private:
	TUniquePtr<PathFollow> m_pBehavior{};

	bool m_PathIsUpToDate{ false };
};

// Leave house action:
// Check if survivor is within bounds of a house that has been visited, if so then yes

class LootHouseAction : public ISurvivorUtilityAction
{
public:

};