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
	// TODO : Blend steering path follow & wander
	TUniquePtr<Wander> m_pWander{};
	TUniquePtr<PathFollow> m_pPathFollow{};
	TUniquePtr<BlendedSteering> m_pBehavior{};

	static constexpr double s_WANDER_TARGET_MIN = 0.0;
	static constexpr double s_WANDER_TARGET_MAX = 6000.0;

	FVector GeneratePos() const;
	float CalculateWeight(const FVector& location, const SurvivorMemory& memory) const;
};