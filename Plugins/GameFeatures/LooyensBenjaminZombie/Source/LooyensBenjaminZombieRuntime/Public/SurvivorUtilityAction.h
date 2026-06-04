#pragma once

struct SurvivorMemory;

class ISurvivorUtilityAction
{
public:
	virtual ~ISurvivorUtilityAction() = default;
	virtual float Evaluate(const SurvivorMemory& memory) = 0;
	virtual void Execute(const SurvivorMemory& memory) = 0;
};