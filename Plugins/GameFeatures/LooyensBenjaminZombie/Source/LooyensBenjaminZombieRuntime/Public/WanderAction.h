#pragma once
#include "SurvivorUtilityAction.h"
#include "SteeringBehaviors.h"

class WanderAction : public ISurvivorUtilityAction
{
public:
	WanderAction();

	virtual float Evaluate(const SurvivorMemory& memory);
	virtual void Execute(SurvivorMemory& memory);

private:
	TUniquePtr<Wander> m_pBehavior{};
};