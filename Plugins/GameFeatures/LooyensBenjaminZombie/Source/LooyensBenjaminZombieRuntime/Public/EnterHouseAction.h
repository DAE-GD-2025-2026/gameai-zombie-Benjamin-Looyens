#pragma once
#include "SurvivorUtilityAction.h"
#include "SteeringBehaviors.h"

class EnterHouseAction : public ISurvivorUtilityAction
{
public:
	EnterHouseAction();

	virtual float Evaluate(const SurvivorMemory& memory);
	virtual void Execute(const SurvivorMemory& memory);

private:
	TUniquePtr<PathFollow> m_pBehavior{};

	bool m_PathIsUpToDate{ false };
};