#pragma once

struct SurvivorMemory;

class ISurvivorUtilityAction
{
public:
	virtual ~ISurvivorUtilityAction() = default;
	virtual float Evaluate(const SurvivorMemory& memory) = 0;
	virtual void Execute(SurvivorMemory& memory) = 0;
	virtual void LateExecute(SurvivorMemory& memory) {} // Optional to overwrite
};