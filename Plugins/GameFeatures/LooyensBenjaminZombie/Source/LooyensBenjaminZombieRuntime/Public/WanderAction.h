#pragma once
#include "SurvivorUtilityAction.h"
#include "SteeringBehaviors.h"

class WanderAction : public ISurvivorUtilityAction
{
public:
	WanderAction();

	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;

private:
	TUniquePtr<Wander> m_pBehavior{};
};