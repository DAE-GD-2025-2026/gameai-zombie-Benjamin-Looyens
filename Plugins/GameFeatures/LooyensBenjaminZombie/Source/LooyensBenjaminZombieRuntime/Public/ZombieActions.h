#pragma once
#include "SurvivorUtilityAction.h"
#include "SteeringBehaviors.h"
#include <Math/Vector.h>

class ShootZombieAction : public ISurvivorUtilityAction
{
public:
	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;
	virtual void LateExecute(SurvivorMemory& memory) override;

private:
	int m_BestWeaponIndex{ -1 };

	// TODO : Figure out these values
	static constexpr float s_MINIMUM_DISTANCE_AWAY = 100.0f; 
	static constexpr float s_MAXIMUM_DISTANCE_AWAY_PISTOL = 600.0f; 
	static constexpr float s_MAXIMUM_DISTANCE_AWAY_SHOTGUN = 300.0f; 
};

class RunAction : public ISurvivorUtilityAction
{
public:
	virtual float Evaluate(const SurvivorMemory& memory) override;
	virtual void Execute(SurvivorMemory& memory) override;
	virtual void LateExecute(SurvivorMemory& memory) override;
};