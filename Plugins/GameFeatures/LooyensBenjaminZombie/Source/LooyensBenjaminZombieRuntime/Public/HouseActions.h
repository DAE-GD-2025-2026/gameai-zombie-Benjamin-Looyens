#pragma once
#include "SurvivorUtilityAction.h"
#include "SteeringBehaviors.h"

// GENERAL:
// HACK : A lot of Actions use a steering behavior at the end of them, maybe I should just make a steering action and call the base class Execute for these
// In the Enter, Exit and Loot actions I kinda do the exact same thing with the "Latest House" thing

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
};

class ExploreHouseAction : public ISurvivorUtilityAction
{
public:
	ExploreHouseAction();

	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;
	virtual void LateExecute(SurvivorMemory& memory) override;

private:
	TUniquePtr<PathFollow> m_pBehavior{};

	HouseMemory* m_pLatestHouse = nullptr;
};