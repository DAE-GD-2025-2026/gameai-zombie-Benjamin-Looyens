#pragma once
#include "SurvivorUtilityAction.h"
#include "SteeringBehaviors.h"

struct ItemMemory;

class CollectItemAction : public ISurvivorUtilityAction
{
public:
	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;
	virtual void LateExecute(SurvivorMemory& memory) override;

private:
	int m_PickupableIndex{ -1 };
};

class HealAction : public ISurvivorUtilityAction
{
public:
	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;
	virtual void LateExecute(SurvivorMemory& memory) override;

private:
	int m_HealableIndex{ -1 };
};

class EatAction : public ISurvivorUtilityAction
{
public:
	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;
	virtual void LateExecute(SurvivorMemory& memory) override;

private:
	int m_EatableIndex{ -1 };
};